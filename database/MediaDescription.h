#ifndef MEDIA_DESCRIPTION_H
#define MEDIA_DESCRIPTION_H

#include <string>

struct MediaDescription {
    std::string mediaUri;
    std::string annotation;
};

MediaDescription readMediaDescription(
    const std::string& xmlPath
);

#endif