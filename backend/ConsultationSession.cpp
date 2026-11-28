#include "ConsultationSession.h"

#include <algorithm>
#include <filesystem>

namespace {

// Topics in the media library (see database/README.md).
const std::vector<std::string> kAllCategories = {"animals", "space", "nature", "music", "food"};

std::string lowercase(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

}  // namespace

ConsultationSession::ConsultationSession(const std::string& sessionId,
                                         const PatientProfile& patient,
                                         std::shared_ptr<const MediaDatabase> mediaDatabase,
                                         const std::string& repoRoot)
    : id(sessionId),
      profile(patient),
      engine(patient),
      database(std::move(mediaDatabase)),
      mediaRoot(repoRoot) {
    current = selectActivity();
}

// Library items for one media type: the child's interests first, then any other
// topic, so a child whose interest has no content still gets something.
std::vector<DistractionMedia> ConsultationSession::findPlayableMedia(
    const std::string& mediaType) const {
    std::vector<std::string> categories;
    for (const auto& interest : profile.getInterests()) {
        std::string category = lowercase(interest);
        if (std::find(categories.begin(), categories.end(), category) == categories.end()) {
            categories.push_back(category);
        }
    }
    for (const auto& category : kAllCategories) {
        if (std::find(categories.begin(), categories.end(), category) == categories.end()) {
            categories.push_back(category);
        }
    }

    std::vector<DistractionMedia> found;
    for (const auto& category : categories) {
        for (auto& media : database->findMedia(mediaType, category, profile.getExactAge())) {
            // Skip items whose file is not on this machine (e.g. videos that
            // must be downloaded separately).
            if (!mediaRoot.empty() &&
                !std::filesystem::exists(std::filesystem::path(mediaRoot) / media.filePath)) {
                continue;
            }
            found.push_back(std::move(media));
        }
    }
    return found;
}

// Ask the engine what to do next; if the library has nothing playable for its
// choice, tell the engine and ask again. Each pass rules out one media type,
// so the loop ends (in Rule 10 manual intervention at the latest).
ActivitySuggestion ConsultationSession::selectActivity() {
    for (int attempt = 0; attempt < 8; ++attempt) {
        EngineDecision decision = engine.generateRecommendation();

        ActivitySuggestion suggestion;
        suggestion.mediaType = decision.recommendedMediaType;
        suggestion.firedRules = decision.firedRules;
        suggestion.rationale = decision.rationale;

        if (decision.recommendedMediaType == "manual_intervention") {
            suggestion.manualIntervention = true;
            return suggestion;
        }

        // Rule 7 / "keep current": reuse what is already showing.
        if (decision.recommendedMediaType == current.mediaType && !current.media.empty()) {
            suggestion.media = current.media;
            return suggestion;
        }

        suggestion.media = findPlayableMedia(decision.recommendedMediaType);
        if (!suggestion.media.empty()) {
            return suggestion;
        }

        engine.markUnavailable(decision.recommendedMediaType);
    }

    // Defensive: should not be reachable.
    ActivitySuggestion fallback;
    fallback.mediaType = "manual_intervention";
    fallback.manualIntervention = true;
    fallback.rationale = "No playable media found; manual physician intervention suggested.";
    return fallback;
}

const ActivitySuggestion& ConsultationSession::recordPhysicianFeedback(const std::string& feedback) {
    engine.applyPhysicianFeedback(feedback);  // throws on invalid input before history changes
    history.push_back(lowercase(feedback));
    updateSystemFacts();
    return current;
}

void ConsultationSession::updateSystemFacts() {
    current = selectActivity();
}

const ActivitySuggestion& ConsultationSession::getCurrentActivity() const {
    return current;
}

const std::vector<std::string>& ConsultationSession::getFacts() const {
    return engine.getFacts();
}

const std::vector<std::string>& ConsultationSession::getFeedbackHistory() const {
    return history;
}

const PatientProfile& ConsultationSession::getProfile() const {
    return profile;
}

const std::string& ConsultationSession::getId() const {
    return id;
}
