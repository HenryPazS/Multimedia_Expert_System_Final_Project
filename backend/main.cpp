#include "InferenceEngine.h"
#include "PatientProfile.h"
#include <iostream>

static void printDecision(const std::string& stepLabel, const EngineDecision& d) {
    std::cout << "=== " << stepLabel << " ===\n";
    std::cout << "Recommended: " << d.recommendedMediaType << "\n";
    std::cout << "Rationale:   " << d.rationale << "\n";
    std::cout << "Fired Rules: ";
    if (d.firedRules.empty()) {
        std::cout << "(none / fallback)";
    } else {
        for (size_t i = 0; i < d.firedRules.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << d.firedRules[i];
        }
    }
    std::cout << "\n\n";
}

int main() {
    PatientProfile child(2.0, {"animals", "colors"}, {"visuals", "video", "audio"});
    InferenceEngine engine(child);

    // Step 1: Initial recommendation
    printDecision("Initial Recommendation", engine.generateRecommendation());

    // Step 2: Physician marks current activity as ineffective
    engine.applyPhysicianFeedback("ineffective");
    printDecision("After 'ineffective' Feedback", engine.generateRecommendation());

    // Step 3: Physician marks current activity (video) as disruptive
    engine.applyPhysicianFeedback("disruptive");
    printDecision("After 'disruptive' Feedback", engine.generateRecommendation());

    return 0;
}