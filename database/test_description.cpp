#include "MediaDatabase.h"
#include "MediaDescription.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void checkDescription(
    const MediaDatabase& database,
    const std::string& mediaType,
    const std::string& category,
    const std::string& title,
    const std::string& expectedWord,
    double age = 2
) {
    auto matches = database.findMedia(mediaType, category, age);

    for (const auto& media : matches) {
        if (media.title != title) {
            continue;
        }

        if (media.descriptionPath.empty()) {
            throw std::runtime_error(
                title + ": missing description path."
            );
        }

        auto description =
            readMediaDescription(media.descriptionPath);

        auto referencedFile =
            std::filesystem::path(media.descriptionPath)
                .parent_path() / description.mediaUri;

        if (!std::filesystem::equivalent(
                referencedFile, media.filePath)) {
            throw std::runtime_error(
                title + ": XML and database files do not match."
            );
        }

        if (description.annotation.find(expectedWord) ==
            std::string::npos) {
            throw std::runtime_error(
                title + ": expected annotation text not found."
            );
        }

        std::cout << "PASS: " << title << "\n"
                  << "Annotation: "
                  << description.annotation << "\n\n";
        return;
    }

    throw std::runtime_error(title + ": record not found.");
}

int main() {
    try {
        MediaDatabase database("database/media.db");

        checkDescription(
            database, "images", "animals",
            "Elephant", "elephant"
        );

        checkDescription(
            database, "music", "nature",
            "Gentle Waves", "waves"
        );

        checkDescription(
            database, "music", "nature",
            "Bird Sounds", "bird"
        );

        checkDescription(
            database, "games", "food",
            "Food Memory Matching Game", "matching", 6
        );

        checkDescription(
            database, "video", "animals",
            "Aquarium Fish", "fish"
        );

        auto elephantMatches = database.findMediaByAnnotation(
            "images", "animals", 2, "ELEPHANT"
        );

        if (elephantMatches.size() != 1 ||
            elephantMatches.front().title != "Elephant") {
            throw std::runtime_error(
                "Expected the XML search to return only Elephant."
            );
        }

        std::cout << "PASS: Case-insensitive XML search found Elephant.\n";

        auto unrelatedMatches = database.findMediaByAnnotation(
            "images", "animals", 2, "spaceship"
        );

        if (!unrelatedMatches.empty()) {
            throw std::runtime_error(
                "Expected no XML matches for spaceship."
            );
        }

        auto spaceImages = database.findMedia("images", "space", 2);

        if (spaceImages.size() != 42) {
            throw std::runtime_error("Expected 42 space images.");
        }

        auto planets = database.findMediaByAnnotation(
            "images", "space", 2, "fantasy planet"
        );

        if (planets.size() != 10) {
            throw std::runtime_error(
                "Expected 10 planets from the XML annotation search."
            );
        }

        std::cout << "PASS: 42 space images retrieved.\n";
        std::cout << "PASS: XML search found 10 fantasy planets.\n";

        std::cout << "PASS: Unmatched XML search returned no results.\n";

        std::cout << "\nAll seven description checks passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "Test failed: " << error.what() << "\n";
        return 1;
    }

    return 0;
}