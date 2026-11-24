#include "InferenceEngine.h"
#include <algorithm>
#include <sstream>

InferenceEngine::InferenceEngine(const PatientProfile& patient)
    : profile(patient),
      ageFuzzy(patient.CalculateFuzzyAge()),
      currentActivity(""),
      previousActivity(""),
      lastRemovedActivity(""),
      lastFeedback(""),
      preferenceIndex(0) {
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
    if (pref == "visuals" || pref == "images") return "images";
    if (pref == "interaction" || pref == "games" || pref == "interactive game") return "games";
    if (pref == "audio" || pref == "music") return "music";
    if (pref == "video") return "video";
    return pref;
}

std::string InferenceEngine::nextAvailableParentMediaType() const {
    const auto& prefs = profile.getMediaPreferences();
    for (size_t i = static_cast<size_t>(preferenceIndex); i < prefs.size(); ++i) {
        std::string mapped = mapPreferenceToMediaType(prefs[i]);
        if (blockedMediaTypes.find(mapped) == blockedMediaTypes.end()) {
            return mapped;
        }
    }
    return "";
}

void InferenceEngine::applyPhysicianFeedback(const std::string& feedback) {
    lastFeedback = feedback;
    addFact("PhysicianFeedback=" + feedback);

    // Rule 3 & Rule 5: Current activity deemed disruptive -> remove from session media
    if (feedback == "disruptive" && !currentActivity.empty()) {
        if (currentActivity == "video") {
            blockedMediaTypes.insert("video");
            lastRemovedActivity = "video";
            previousActivity = currentActivity;
            currentActivity = "";
            addFact("RemovedDisruptive=video");
        } else if (currentActivity == "games" || currentActivity == "interactive game") {
            blockedMediaTypes.insert("games");
            lastRemovedActivity = "games";
            previousActivity = currentActivity;
            currentActivity = "";
            addFact("RemovedDisruptive=games");
        } else {
            blockedMediaTypes.insert(currentActivity);
            lastRemovedActivity = currentActivity;
            previousActivity = currentActivity;
            currentActivity = "";
            addFact("RemovedDisruptive=" + lastRemovedActivity);
        }
    } else if (feedback == "ineffective") {
        previousActivity = currentActivity;
        if (preferenceIndex + 1 < static_cast<int>(profile.getMediaPreferences().size())) {
            ++preferenceIndex;
        }
    }
}

void InferenceEngine::evaluateRules() {
    // Forward-chaining evaluation across Rules 1-10
    bool isVeryYoung = ageFuzzy.veryYoung > FUZZY_THRESHOLD;
    bool isYoung = ageFuzzy.young > FUZZY_THRESHOLD;
    bool isNotYoung = ageFuzzy.notYoung > FUZZY_THRESHOLD;

    if (isVeryYoung && profile.hasPreference("visuals")) {
        addFact("Rule1Applicable:simple_images");
    }
    if (isNotYoung && (profile.hasPreference("interaction") || profile.hasPreference("games"))) {
        addFact("Rule2Applicable:easy_interactive_games");
    }
    if (lastFeedback == "disruptive" && lastRemovedActivity == "video") {
        addFact("Rule3Fired:video_removed");
        addFact("Rule4Applicable:suggest_music");
    }
    if (lastFeedback == "disruptive" && lastRemovedActivity == "games") {
        addFact("Rule5Fired:game_removed");
        addFact("Rule6Applicable:suggest_music_or_images");
    }
    if (lastFeedback == "effective" && !currentActivity.empty()) {
        addFact("Rule7Fired:maintain_current_activity");
    }
    if (lastFeedback == "ineffective") {
        addFact("Rule8Fired:select_next_preferred_media");
    }
    if (isYoung && (profile.hasPreference("audio") || profile.hasPreference("music"))) {
        addFact("Rule9Applicable:suggest_music");
    }
}

EngineDecision InferenceEngine::generateRecommendation() {
    evaluateRules();

    EngineDecision decision;
    bool isVeryYoung = ageFuzzy.veryYoung > FUZZY_THRESHOLD;
    bool isYoung = ageFuzzy.young > FUZZY_THRESHOLD;
    bool isNotYoung = ageFuzzy.notYoung > FUZZY_THRESHOLD;

    // Rule 7: Maintain current activity if effective
    if (lastFeedback == "effective" && !currentActivity.empty()) {
        decision.recommendedMediaType = currentActivity;
        decision.firedRules.push_back("Rule 7");
        decision.rationale = "Physician marked current activity as effective; maintaining it.";
        return decision;
    }

    // Rule 4: Previous video removed due to disruption -> suggest music (if music not blocked)
    if (lastFeedback == "disruptive" && lastRemovedActivity == "video" &&
        blockedMediaTypes.find("music") == blockedMediaTypes.end()) {
        currentActivity = "music";
        decision.recommendedMediaType = "music";
        decision.firedRules = {"Rule 3", "Rule 4"};
        decision.rationale = "Video was disruptive and removed; suggesting music.";
        addFact("Recommended=" + currentActivity);
        return decision;
    }

    // Rule 6: Previous interactive game removed due to disruption -> suggest music or images
    if (lastFeedback == "disruptive" && lastRemovedActivity == "games") {
        if (blockedMediaTypes.find("music") == blockedMediaTypes.end()) {
            currentActivity = "music";
            decision.recommendedMediaType = "music";
            decision.firedRules = {"Rule 5", "Rule 6"};
            decision.rationale = "Interactive game was disruptive and removed; suggesting music.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }
        if (blockedMediaTypes.find("images") == blockedMediaTypes.end()) {
            currentActivity = "images";
            decision.recommendedMediaType = "images";
            decision.firedRules = {"Rule 5", "Rule 6"};
            decision.rationale = "Interactive game was disruptive and removed; suggesting simple images.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }
    }

    // Rule 8: Ineffective -> select next preferred media type from parent form
    if (lastFeedback == "ineffective") {
        std::string nextType = nextAvailableParentMediaType();
        if (!nextType.empty()) {
            currentActivity = nextType;
            decision.recommendedMediaType = nextType;
            decision.firedRules.push_back("Rule 8");
            decision.rationale = "Previous activity ineffective; advancing to next preferred media type.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }
    }

    // Initial / fuzzy-guided rules (Rules 1, 2, 9) when no feedback override applies
    if (currentActivity.empty()) {
        if (isVeryYoung && profile.hasPreference("visuals") &&
            blockedMediaTypes.find("images") == blockedMediaTypes.end()) {
            currentActivity = "images";
            decision.recommendedMediaType = "images";
            decision.firedRules.push_back("Rule 1");
            decision.rationale = "Child is very young (>0.5 membership) and prefers visuals; recommending simple images.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }

        if (isNotYoung && (profile.hasPreference("interaction") || profile.hasPreference("games")) &&
            blockedMediaTypes.find("games") == blockedMediaTypes.end()) {
            currentActivity = "games";
            decision.recommendedMediaType = "games";
            decision.firedRules.push_back("Rule 2");
            decision.rationale = "Child is not young (>0.5 membership) and prefers interaction; recommending easy interactive games.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }

        if (isYoung && (profile.hasPreference("audio") || profile.hasPreference("music")) &&
            blockedMediaTypes.find("music") == blockedMediaTypes.end()) {
            currentActivity = "music";
            decision.recommendedMediaType = "music";
            decision.firedRules.push_back("Rule 9");
            decision.rationale = "Child is young (>0.5 membership) and prefers audio; recommending music.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }

        // Fallback to first unblocked parent preference if available
        std::string fallbackType = nextAvailableParentMediaType();
        if (!fallbackType.empty()) {
            currentActivity = fallbackType;
            decision.recommendedMediaType = fallbackType;
            decision.rationale = "Selected next unblocked media preference from parent form.";
            addFact("Recommended=" + currentActivity);
            return decision;
        }
    }

    // Rule 10: All preferred/available media types exhausted or marked disruptive
    decision.recommendedMediaType = "manual_intervention";
    decision.firedRules.push_back("Rule 10");
    decision.rationale = "Preferred or available media options are exhausted or disruptive; manual physician intervention suggested.";
    addFact("Rule10Fired:manual_intervention");
    return decision;
}

std::string InferenceEngine::getCurrentActivity() const {
    return currentActivity;
}

const std::vector<std::string>& InferenceEngine::getFacts() const {
    return facts;
}