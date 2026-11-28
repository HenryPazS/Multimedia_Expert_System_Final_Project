#include "MediaDescription.h"

#include <tinyxml2.h>
#include <sstream>
#include <stdexcept>

MediaDescription readMediaDescription(const std::string& xmlPath) {
    tinyxml2::XMLDocument document;

    if (document.LoadFile(xmlPath.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error(
            "Cannot read XML file: " + xmlPath +
            " - " + document.ErrorStr()
        );
    }

    auto requireChild = [](tinyxml2::XMLElement* parent,
                           const char* name) {
        auto* child = parent->FirstChildElement(name);

        if (!child) {
            throw std::runtime_error(
                std::string("Missing XML element: ") + name
            );
        }

        return child;
    };

    auto* root = document.FirstChildElement("Mpeg7");

    if (!root) {
        throw std::runtime_error("Missing Mpeg7 root element.");
    }

    const char* namespaceUri = root->Attribute("xmlns");

    if (!namespaceUri ||
        std::string(namespaceUri) != "urn:mpeg:mpeg7:schema:2001") {
        throw std::runtime_error("Unexpected MPEG-7 namespace.");
    }

    auto* description = requireChild(root, "Description");

    tinyxml2::XMLElement* uri = nullptr;
    tinyxml2::XMLElement* freeText = nullptr;

    const char* descriptionType = description->Attribute("xsi:type");

    if (!descriptionType) {
        throw std::runtime_error("Missing description type.");
    }

    if (std::string(descriptionType) == "CreationDescriptionType") {
        // Creation-information description used for the HTML game.
        auto* information =
            requireChild(description, "CreationInformation");

        auto* creation = requireChild(information, "Creation");
        auto* abstract = requireChild(creation, "Abstract");
        freeText = requireChild(abstract, "FreeTextAnnotation");

        auto* related = requireChild(information, "RelatedMaterial");
        auto* locator = requireChild(related, "MediaLocator");
        uri = requireChild(locator, "MediaUri");

    } else if (std::string(descriptionType) == "ContentEntityType") {
        // Existing image, audio, and video descriptions.
        auto* content = requireChild(description, "MultimediaContent");
        const char* contentType = content->Attribute("xsi:type");

        if (!contentType) {
            throw std::runtime_error("Missing multimedia content type.");
        }

        tinyxml2::XMLElement* mediaElement = nullptr;
        const std::string type = contentType;

        if (type == "ImageType") {
            mediaElement = requireChild(content, "Image");
        } else if (type == "AudioType") {
            mediaElement = requireChild(content, "Audio");
        } else if (type == "VideoType") {
            mediaElement = requireChild(content, "Video");
        } else {
            throw std::runtime_error(
                "Unsupported multimedia content type: " + type
            );
        }

        auto* locator = requireChild(mediaElement, "MediaLocator");
        uri = requireChild(locator, "MediaUri");

        auto* annotation = requireChild(mediaElement, "TextAnnotation");
        freeText = requireChild(annotation, "FreeTextAnnotation");

    } else {
        throw std::runtime_error(
            std::string("Unsupported description type: ") +
            descriptionType
        );
    }

    // Remove indentation and extra whitespace from XML text.
    auto normalizeText = [](const char* text) {
        std::istringstream input(text ? text : "");
        std::string word;
        std::string normalized;

        while (input >> word) {
            if (!normalized.empty()) {
                normalized += " ";
            }
            normalized += word;
        }

        return normalized;
    };

    MediaDescription result;
    result.mediaUri = normalizeText(uri->GetText());
    result.annotation = normalizeText(freeText->GetText());

    if (result.mediaUri.empty() || result.annotation.empty()) {
        throw std::runtime_error(
            "Media location and annotation must not be empty."
        );
    }

    return result;
}