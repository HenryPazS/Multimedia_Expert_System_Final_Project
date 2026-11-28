#include "InferenceEngine.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

InferenceEngine::InferenceEngine(const PatientProfile& patient)
    : profile(patient),
      ageFuzzy(patient.CalculateFuzzyAge()),
      currentActivity(""),
      lastRemovedActivity(""),
      lastFeedback(""),
      feedbackPending(false) {
    std::ostringstream ageFact;
    ageFact << "Age=" << profile.getExactAge()
            << " (very_young=" << ageFuzzy.veryYoung
            << ", young=" << ageFuzzy.young
            << ", not_young=" << ageFuzzy.notYoung << ")";
    addFact(ageFact.str());

    for (const auto& pref : profile.getMediaPreferences()) {
        addFact("ParentPref=" + pref);
    }
}

void InferenceEngine::addFact(const std::string& fact) {
    if (std::find(facts.begin(), facts.end(), fact) == facts.end()) {
        facts.push_back(fact);
    }
}

std::string InferenceEngine::mapPreferenceToMediaType(const std::string& pref) const {
    std::string p = pref;
    std::transform(p.begin(), p.end(), p.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (p == "visuals" || p == "visual" || p == "images" || p == "image") return "images";
    if (p == "interaction" || p == "games" || p == "game" || p == "interactive game") return "games";
    if (p == "audio" || p == "music" || p == "sound") return "music";
    if (p == "video") return "video";
    return p;
}

bool InferenceEngine::isAvailable(const std::string& mediaType) const {
    return blockedMediaTypes.count(mediaType) == 0 &&
           ineffectiveMediaTypes.count(mediaType) == 0 &&
           unavailableMediaTypes.count(mediaType) == 0;
}

// First type in the parent's preference order that is still usable.
std::string InferenceEngine::nextAvailableParentMediaType() const {
    for (const auto& pref : profile.getMediaPreferences()) {
        std::string mapped = mapPreferenceToMediaType(pref);
        if (isAvailable(mapped)) {
            return mapped;
        }
    }
    return "";
}

void InferenceEngine::applyPhysicianFeedback(const std::string& rawFeedback) {
    std::string feedback = rawFeedback;
    std::transform(feedback.begin(), feedback.end(), feedback.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (feedback != "effective" && feedback != "ineffective" && feedback != "disruptive") {
        throw std::invalid_argument(
            "Feedback must be 'effective', 'ineffective' or 'disruptive'.");
    }

    addFact("PhysicianFeedback=" + feedback);

    // Feedback is about the activity on screen. With nothing on screen
    // (nothing recommended yet, or manual intervention already requested)
    // there is nothing to judge, so record it and change nothing.
    if (currentActivity.empty()) {
        addFact("FeedbackIgnored=no_current_activity");
        return;
    }

    lastFeedback = feedback;
    feedbackPending = true;

    if (feedback == "disruptive") {
        // Rules 3 and 5: remove the disruptive media type from the session.
        blockedMediaTypes.insert(currentActivity);
        lastRemovedActivity = currentActivity;
        addFact("RemovedDisruptive=" + currentActivity);
        currentActivity.clear();
    } else if (feedback == "ineffective") {
        // Rule 8: never go back to something that did not work.
        ineffectiveMediaTypes.insert(currentActivity);
        addFact("Ineffective=" + currentActivity);
        currentActivity.clear();
    }
    // "effective" keeps the current activity (Rule 7).
}

void InferenceEngine::markUnavailable(const std::string& mediaType) {
    unavailableMediaTypes.insert(mediaType);
    addFact("Unavailable=" + mediaType);
    if (currentActivity == mediaType) {
        currentActivity.clear();
    }
}

void InferenceEngine::evaluateRules() {
    // Forward-chaining pass: derive facts from the current state.
    bool isVeryYoung = ageFuzzy.veryYoung > FUZZY_THRESHOLD;
    bool isYoung = ageFuzzy.young > FUZZY_THRESHOLD;
    bool isNotYoung = ageFuzzy.notYoung > FUZZY_THRESHOLD;

    if (isVeryYoung && profile.hasPreference("visuals")) {
        addFact("Rule1Applicable:simple_images");
    }
    if (isNotYoung && profile.hasPreference("interaction")) {
        addFact("Rule2Applicable:easy_interactive_games");
    }
    if (feedbackPending && lastFeedback == "disruptive" && lastRemovedActivity == "video") {
        addFact("Rule3Fired:video_removed");
        addFact("Rule4Applicable:suggest_music");
    }
    if (feedbackPending && lastFeedback == "disruptive" && lastRemovedActivity == "games") {
        addFact("Rule5Fired:game_removed");
        addFact("Rule6Applicable:suggest_music_or_images");
    }
    if (feedbackPending && lastFeedback == "effective" && !currentActivity.empty()) {
        addFact("Rule7Fired:maintain_current_activity");
    }
    if (feedbackPending && lastFeedback == "ineffective") {
        addFact("Rule8Fired:select_next_preferred_media");
    }
    if (isYoung && profile.hasPreference("audio")) {
        addFact("Rule9Applicable:suggest_music");
    }
}

EngineDecision InferenceEngine::recommend(const std::string& mediaType,
                                          const std::vector<std::string>& rules,
                                          const std::string& rationale) {
    currentActivity = mediaType;
    feedbackPending = false;
    addFact("Recommended=" + mediaType);
    EngineDecision decision;
    decision.recommendedMediaType = mediaType;
    decision.firedRules = rules;
    decision.rationale = rationale;
    return decision;
}

// Rule 10: nothing preferred is left.
EngineDecision InferenceEngine::manualIntervention() {
    currentActivity.clear();
    feedbackPending = false;
    EngineDecision decision;
    decision.recommendedMediaType = "manual_intervention";
    decision.firedRules.push_back("Rule 10");
    decision.rationale =
        "Preferred or available media options are exhausted or disruptive; "
        "manual physician intervention suggested.";
    addFact("Rule10Fired:manual_intervention");
    return decision;
}

EngineDecision InferenceEngine::generateRecommendation() {
    evaluateRules();

    bool isVeryYoung = ageFuzzy.veryYoung > FUZZY_THRESHOLD;
    bool isYoung = ageFuzzy.young > FUZZY_THRESHOLD;
    bool isNotYoung = ageFuzzy.notYoung > FUZZY_THRESHOLD;

    // Nothing new to react to and something is already showing: keep it.
    if (!currentActivity.empty() && !feedbackPending) {
        EngineDecision decision;
        decision.recommendedMediaType = currentActivity;
        decision.rationale = "No new physician feedback; keeping current activity.";
        return decision;
    }

    // Rule 7: effective -> maintain the current activity.
    if (!currentActivity.empty() && lastFeedback == "effective") {
        feedbackPending = false;
        EngineDecision decision;
        decision.recommendedMediaType = currentActivity;
        decision.firedRules.push_back("Rule 7");
        decision.rationale = "Physician marked current activity as effective; maintaining it.";
        return decision;
    }

    if (feedbackPending && lastFeedback == "disruptive") {
        // Rules 3-4: a removed video is replaced by music.
        if (lastRemovedActivity == "video" && isAvailable("music")) {
            return recommend("music", {"Rule 3", "Rule 4"},
                             "Video was disruptive and removed; suggesting music.");
        }
        // Rules 5-6: a removed game is replaced by music or images.
        if (lastRemovedActivity == "games") {
            if (isAvailable("music")) {
                return recommend("music", {"Rule 5", "Rule 6"},
                                 "Interactive game was disruptive and removed; suggesting music.");
            }
            if (isAvailable("images")) {
                return recommend("images", {"Rule 5", "Rule 6"},
                                 "Interactive game was disruptive and removed; suggesting simple images.");
            }
        }
        // Music/images being disruptive, or the replacement being unavailable:
        // go on to the parent's other preferences below.
    }

    // Rule 8: ineffective -> next preferred media type from the parent form.
    if (feedbackPending && lastFeedback == "ineffective") {
        std::string nextType = nextAvailableParentMediaType();
        if (!nextType.empty()) {
            return recommend(nextType, {"Rule 8"},
                             "Previous activity ineffective; advancing to next preferred media type.");
        }
        return manualIntervention();
    }

    // No activity yet (start of session, or a replacement was unavailable):
    // fuzzy-guided rules 1, 2 and 9, then the parent's preference order.
    if (isVeryYoung && profile.hasPreference("visuals") && isAvailable("images")) {
        return recommend("images", {"Rule 1"},
                         "Child is very young (>0.5 membership) and prefers visuals; recommending simple images.");
    }
    if (isNotYoung && profile.hasPreference("interaction") && isAvailable("games")) {
        return recommend("games", {"Rule 2"},
                         "Child is not young (>0.5 membership) and prefers interaction; recommending easy interactive games.");
    }
    if (isYoung && profile.hasPreference("audio") && isAvailable("music")) {
        return recommend("music", {"Rule 9"},
                         "Child is young (>0.5 membership) and prefers audio; recommending music.");
    }

    std::string fallbackType = nextAvailableParentMediaType();
    if (!fallbackType.empty()) {
        return recommend(fallbackType, {},
                         "Selected next unblocked media preference from parent form.");
    }

    return manualIntervention();
}

std::string InferenceEngine::getCurrentActivity() const {
    return currentActivity;
}

const std::vector<std::string>& InferenceEngine::getFacts() const {
    return facts;
}

const std::set<std::string>& InferenceEngine::getBlockedMediaTypes() const {
    return blockedMediaTypes;
}
