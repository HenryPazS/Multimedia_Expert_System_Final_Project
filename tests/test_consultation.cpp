// Tests for the inference-engine fallbacks and ConsultationSession.
// Run from the repository root (needs database/media.db).

#include "ConsultationSession.h"
#include "InferenceEngine.h"
#include "MediaDatabase.h"
#include "PatientProfile.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static int failures = 0;

static void check(bool condition, const std::string& what) {
    if (!condition) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

static bool hasRule(const EngineDecision& d, const std::string& rule) {
    for (const auto& r : d.firedRules) if (r == rule) return true;
    return false;
}

// The worked example from the proposal.
static void testProposalExample() {
    InferenceEngine e(PatientProfile(2.0, {"animals"}, {"visuals", "video"}));
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "images" && hasRule(d, "Rule 1"), "rule 1: very young + visuals -> images");

    e.applyPhysicianFeedback("ineffective");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "video" && hasRule(d, "Rule 8"), "rule 8: ineffective -> next preferred (video)");

    e.applyPhysicianFeedback("disruptive");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "music" && hasRule(d, "Rule 3") && hasRule(d, "Rule 4"),
          "rules 3+4: disruptive video removed -> music");
}

static void testEffectiveKeepsActivity() {
    InferenceEngine e(PatientProfile(2.0, {}, {"visuals", "video"}));
    e.generateRecommendation();
    e.applyPhysicianFeedback("effective");
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "images" && hasRule(d, "Rule 7"), "rule 7: effective keeps current activity");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "images", "asking again without feedback keeps the activity");
}

static void testDisruptiveGame() {
    // Rules 5+6: game removed -> music.
    InferenceEngine e(PatientProfile(10.0, {}, {"interaction", "video"}));
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "games" && hasRule(d, "Rule 2"), "rule 2: not young + interaction -> games");
    e.applyPhysicianFeedback("disruptive");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "music" && hasRule(d, "Rule 6"), "rule 6: disruptive game -> music");

    // Music unavailable -> images.
    InferenceEngine e2(PatientProfile(10.0, {}, {"interaction"}));
    e2.generateRecommendation();
    e2.markUnavailable("music");
    e2.applyPhysicianFeedback("disruptive");
    d = e2.generateRecommendation();
    check(d.recommendedMediaType == "images" && hasRule(d, "Rule 6"), "rule 6: game removed, no music -> images");

    // Neither -> manual intervention.
    InferenceEngine e3(PatientProfile(10.0, {}, {"interaction"}));
    e3.generateRecommendation();
    e3.markUnavailable("music");
    e3.markUnavailable("images");
    e3.applyPhysicianFeedback("disruptive");
    d = e3.generateRecommendation();
    check(d.recommendedMediaType == "manual_intervention" && hasRule(d, "Rule 10"),
          "rule 10: game removed, nothing left -> manual intervention");
}

static void testIneffectiveExhausted() {
    // The last preference being ineffective must not repeat it.
    InferenceEngine e(PatientProfile(2.0, {}, {"visuals", "video"}));
    e.generateRecommendation();
    e.applyPhysicianFeedback("ineffective");
    e.generateRecommendation();  // video
    e.applyPhysicianFeedback("ineffective");
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "manual_intervention" && hasRule(d, "Rule 10"),
          "rule 10: every preference ineffective -> manual intervention");
}

static void testDisruptiveEverything() {
    InferenceEngine e(PatientProfile(2.0, {}, {"video"}));
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "video", "single preference recommended first");
    e.applyPhysicianFeedback("disruptive");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "music", "video removed -> music even if not preferred");
    e.applyPhysicianFeedback("disruptive");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "manual_intervention", "music removed too -> manual intervention");
}

static void testBadInputAndOddOrder() {
    InferenceEngine e(PatientProfile(2.0, {}, {"visuals"}));

    bool threw = false;
    try { e.applyPhysicianFeedback("great"); } catch (const std::invalid_argument&) { threw = true; }
    check(threw, "unknown feedback is rejected");

    // Feedback before anything is recommended changes nothing.
    e.applyPhysicianFeedback("disruptive");
    auto d = e.generateRecommendation();
    check(d.recommendedMediaType == "images", "feedback before first recommendation is ignored");

    // Feedback after manual intervention is ignored, not an error.
    e.applyPhysicianFeedback("ineffective");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "manual_intervention", "only preference ineffective -> manual");
    e.applyPhysicianFeedback("effective");
    d = e.generateRecommendation();
    check(d.recommendedMediaType == "manual_intervention", "feedback during manual intervention is ignored");

    // Case-insensitive, and form synonyms ("images", "music") behave like visuals/audio.
    InferenceEngine e2(PatientProfile(2.0, {}, {"images", "music"}));
    d = e2.generateRecommendation();
    check(d.recommendedMediaType == "images" && hasRule(d, "Rule 1"), "'images' counts as visuals");
    e2.applyPhysicianFeedback("DISRUPTIVE");
    d = e2.generateRecommendation();
    check(d.recommendedMediaType == "music", "feedback is case-insensitive");
}

static void testSessionWithDatabase() {
    auto database = std::make_shared<MediaDatabase>("database/media.db");

    // Without a media root, every record counts as playable.
    ConsultationSession s("t1", PatientProfile(2.0, {"space"}, {"visuals", "video"}), database);
    check(s.getCurrentActivity().mediaType == "images", "session starts with images");
    check(!s.getCurrentActivity().media.empty(), "session returns matching media");
    check(s.getCurrentActivity().media.front().category == "space", "interest category listed first");

    s.recordPhysicianFeedback("ineffective");
    check(s.getCurrentActivity().mediaType == "video" && !s.getCurrentActivity().media.empty(), "session moves to video");
    s.recordPhysicianFeedback("disruptive");
    check(s.getCurrentActivity().mediaType == "music" && !s.getCurrentActivity().media.empty(), "session moves to music");
    check(s.getFeedbackHistory().size() == 2, "feedback history recorded");
    s.recordPhysicianFeedback("disruptive");
    check(s.getCurrentActivity().manualIntervention && s.getCurrentActivity().media.empty(),
          "session reaches manual intervention");

    bool threw = false;
    try { s.recordPhysicianFeedback("nope"); } catch (const std::invalid_argument&) { threw = true; }
    check(threw && s.getFeedbackHistory().size() == 3, "invalid feedback rejected, history untouched");

    // Interests that are not library topics still get media.
    ConsultationSession s2("t2", PatientProfile(2.0, {"colors"}, {"visuals"}), database);
    check(!s2.getCurrentActivity().media.empty(), "unknown interest falls back to any topic");

    // A child outside every media age range: nothing playable -> manual intervention.
    ConsultationSession s3("t3", PatientProfile(17.0, {}, {"visuals", "video"}), database);
    check(s3.getCurrentActivity().manualIntervention, "no media for the age -> manual intervention");

    // A media root where the video files are missing: video is skipped, not offered.
    ConsultationSession s4("t4", PatientProfile(2.0, {"space"}, {"video", "audio"}), database, "build/does-not-exist");
    check(s4.getCurrentActivity().manualIntervention, "all files missing -> manual intervention");
}

int main() {
    try {
        testProposalExample();
        testEffectiveKeepsActivity();
        testDisruptiveGame();
        testIneffectiveExhausted();
        testDisruptiveEverything();
        testBadInputAndOddOrder();
        testSessionWithDatabase();
    } catch (const std::exception& error) {
        std::cerr << "Test crashed: " << error.what() << "\n";
        return 1;
    }

    if (failures == 0) {
        std::cout << "All consultation tests passed.\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed.\n";
    return 1;
}
