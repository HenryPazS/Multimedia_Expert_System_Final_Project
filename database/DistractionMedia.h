#ifndef DISTRACTION_MEDIA_H
#define DISTRACTION_MEDIA_H

#include <string>

struct MediaDescriptions {
    std::string mediaType;
    std::string category;
    double minAge;
    double maxAge;
};

class DistractionMedia {
public:
    int id;
    std::string title;
    std::string mediaType;
    std::string category;
    double minAge;
    double maxAge;
    std::string filePath;
    std::string creator;
    std::string sourceUrl;
    std::string license;
    bool hasAudio = false;
    std::string descriptionPath;

    MediaDescriptions getDescriptions() const {
        return {mediaType, category, minAge, maxAge};
    }
};

#endif