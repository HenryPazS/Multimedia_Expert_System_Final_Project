#include "MediaDatabase.h"
#include "../backend/InferenceEngine.h"
#include "../backend/PatientProfile.h"

#include <iostream>
#include <stdexcept>

int main() {
    try {
        PatientProfile child(
            2.0,
            {"space"},
            {"video"}
        );

        InferenceEngine engine(child);
        MediaDatabase database("database/media.db");

        auto decision = engine.generateRecommendation();

        std::cout << "Recommended media: "
                  << decision.recommendedMediaType << "\n";

        std::cout << "Reason: "
                  << decision.rationale << "\n";

        if (decision.recommendedMediaType != "video") {
            throw std::runtime_error(
                "Expected video for this patient profile."
            );
        }

        auto matches = database.findMedia(
            decision.recommendedMediaType,
            child.getInterests().at(0),
            child.getExactAge()
        );

        std::cout << "\nMatching activities: "
                  << matches.size() << "\n";

        for (const auto& media : matches) {
            std::cout << media.title << " | "
                      << media.filePath << "\n";
        }

        if (matches.empty()) {
            throw std::runtime_error(
                "Expected at least one matching video."
            );
        }

        std::cout << "\nIntegration check passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "Integration failed: "
                  << error.what() << "\n";
        return 1;
    }

    return 0;
}