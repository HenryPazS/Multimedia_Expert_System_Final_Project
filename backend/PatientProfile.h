#ifndef PATIENT_PROFILE_H
#define PATIENT_PROFILE_H

#include <string>
#include <vector>

struct FuzzyAgeMemberships {
    double young = 0.0;
    double notYoung = 0.0;
    double veryYoung = 0.0;
};

class PatientProfile {
private:
    double exactAge;
    std::vector<std::string> interests;
    std::vector<std::string> mediaPreferences; // ordered preference list, e.g., {"visuals", "video", "audio", "interaction"}

public:
    PatientProfile(double age,
                   const std::vector<std::string>& childInterests,
                   const std::vector<std::string>& preferences);

    double getExactAge() const;
    FuzzyAgeMemberships CalculateFuzzyAge() const;
    const std::vector<std::string>& getMediaPreferences() const;
    const std::vector<std::string>& getInterests() const;
    bool hasPreference(const std::string& pref) const;
};

#endif