#include "PatientProfile.h"
#include <algorithm>
#include <cctype>
#include <cmath>

// Accept the different words the form/UI may use for the same preference.
static std::string canonicalPreference(std::string pref) {
    std::transform(pref.begin(), pref.end(), pref.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (pref == "images" || pref == "image" || pref == "visual") return "visuals";
    if (pref == "music" || pref == "sound") return "audio";
    if (pref == "games" || pref == "game" || pref == "interactive game") return "interaction";
    return pref;
}

PatientProfile::PatientProfile(double age,const std::vector<std::string>& childInterests,const std::vector<std::string>& preferences): exactAge(age), interests(childInterests), mediaPreferences(preferences) {}

double PatientProfile::getExactAge() const {
    return exactAge;
}

FuzzyAgeMemberships PatientProfile::CalculateFuzzyAge() const {
    FuzzyAgeMemberships memberships;

    // "young" is a fuzzy set over 0-12: 1.0 for age <= 3, decreasing linearly to 0.0 at age 12
    if (exactAge <= 3.0) {
        memberships.young = 1.0;
    } else if (exactAge >= 12.0) {
        memberships.young = 0.0;
    } else {
        memberships.young = (12.0 - exactAge) / (12.0 - 3.0);
    }

    // "not young" is the complement: 1 - mu_young
    memberships.notYoung = 1.0 - memberships.young;

    // "very young" uses hedge CON(A): [mu_young]^2
    memberships.veryYoung = std::pow(memberships.young, 2.0);

    return memberships;
}

const std::vector<std::string>& PatientProfile::getMediaPreferences() const {
    return mediaPreferences;
}

const std::vector<std::string>& PatientProfile::getInterests() const {
    return interests;
}

bool PatientProfile::hasPreference(const std::string& pref) const {
    const std::string wanted = canonicalPreference(pref);
    return std::any_of(mediaPreferences.begin(), mediaPreferences.end(),
                       [&](const std::string& p) { return canonicalPreference(p) == wanted; });
}