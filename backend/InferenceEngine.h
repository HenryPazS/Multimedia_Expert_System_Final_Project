#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include "PatientProfile.h"
#include <set>
#include <string>
#include <vector>

struct EngineDecision {
    std::string recommendedMediaType;
    std::vector<std::string> firedRules;
    std::string rationale;
};

class InferenceEngine {
private:
    PatientProfile profile;
    FuzzyAgeMemberships ageFuzzy;
    std::string currentActivity;
    std::string lastRemovedActivity;
    std::string lastFeedback;
    bool feedbackPending;                         // feedback not yet turned into a recommendation
    std::set<std::string> blockedMediaTypes;      // removed as disruptive (Rules 3, 5)
    std::set<std::string> ineffectiveMediaTypes;  // already tried and judged ineffective (Rule 8)
    std::set<std::string> unavailableMediaTypes;  // nothing usable in the media library
    std::vector<std::string> facts;
    static constexpr double FUZZY_THRESHOLD = 0.5;

    std::string mapPreferenceToMediaType(const std::string& pref) const;
    bool isAvailable(const std::string& mediaType) const;
    std::string nextAvailableParentMediaType() const;
    EngineDecision recommend(const std::string& mediaType,
                             const std::vector<std::string>& rules,
                             const std::string& rationale);
    EngineDecision manualIntervention();

public:
    explicit InferenceEngine(const PatientProfile& patient);

    void addFact(const std::string& fact);
    // Accepts "effective", "ineffective" or "disruptive" (case-insensitive).
    // Throws std::invalid_argument for anything else.
    void applyPhysicianFeedback(const std::string& feedback);
    // Tell the engine the media library has nothing usable for this type, so it
    // is never recommended again this session.
    void markUnavailable(const std::string& mediaType);
    void evaluateRules();
    EngineDecision generateRecommendation();

    std::string getCurrentActivity() const;
    const std::vector<std::string>& getFacts() const;
    const std::set<std::string>& getBlockedMediaTypes() const;
};

#endif
