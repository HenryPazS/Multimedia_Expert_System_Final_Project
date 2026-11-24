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
    std::string previousActivity;
    std::string lastRemovedActivity;
    std::string lastFeedback;
    std::set<std::string> blockedMediaTypes; 
    std::vector<std::string> facts;
    int preferenceIndex;
    static constexpr double FUZZY_THRESHOLD = 0.5;

    std::string mapPreferenceToMediaType(const std::string& pref) const;
    std::string nextAvailableParentMediaType() const;

public:
    explicit InferenceEngine(const PatientProfile& patient);

    void addFact(const std::string& fact);
    void applyPhysicianFeedback(const std::string& feedback); 
    void evaluateRules();
    EngineDecision generateRecommendation();

    std::string getCurrentActivity() const;
    const std::vector<std::string>& getFacts() const;
};

#endif