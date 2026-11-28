#ifndef CONSULTATION_SESSION_H
#define CONSULTATION_SESSION_H

#include "DistractionMedia.h"
#include "InferenceEngine.h"
#include "MediaDatabase.h"
#include "PatientProfile.h"

#include <memory>
#include <string>
#include <vector>

// What the frontend should show right now.
struct ActivitySuggestion {
    std::string mediaType;                 // images, music, video, games or manual_intervention
    bool manualIntervention = false;       // Rule 10: the physician takes over
    std::vector<std::string> firedRules;
    std::string rationale;
    std::vector<DistractionMedia> media;   // matching library items; media[0] is the featured one
};

// One consultation: owns the child's profile and the inference engine, asks the
// media database for content, and turns physician feedback into the next activity.
class ConsultationSession {
private:
    std::string id;
    PatientProfile profile;
    InferenceEngine engine;
    std::shared_ptr<const MediaDatabase> database;
    std::string mediaRoot;                 // repo root; when set, media whose file is missing are skipped
    ActivitySuggestion current;
    std::vector<std::string> history;      // feedback received, in order

    std::vector<DistractionMedia> findPlayableMedia(const std::string& mediaType) const;
    ActivitySuggestion selectActivity();

public:
    ConsultationSession(const std::string& sessionId,
                        const PatientProfile& patient,
                        std::shared_ptr<const MediaDatabase> mediaDatabase,
                        const std::string& repoRoot = "");

    // Physician feedback ("effective" / "ineffective" / "disruptive"); returns the
    // activity to show next. Throws std::invalid_argument for unknown feedback.
    const ActivitySuggestion& recordPhysicianFeedback(const std::string& feedback);

    // Re-run the rules over the current facts and refresh the suggestion.
    void updateSystemFacts();

    const ActivitySuggestion& getCurrentActivity() const;
    const std::vector<std::string>& getFacts() const;
    const std::vector<std::string>& getFeedbackHistory() const;
    const PatientProfile& getProfile() const;
    const std::string& getId() const;
};

#endif
