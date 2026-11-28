#include "MediaDatabase.h"

#include <iostream>
#include <stdexcept>

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main() {
    try {
        MediaDatabase database("database/media.db");

        auto images = database.findMedia("images", "animals", 2);
        require(images.size() == 10, "Expected 10 animal images.");

        for (const auto& image : images) {
            require(!image.hasAudio, "Images should not have audio.");
        }
        std::cout << "PASS: 10 animal images without audio\n";

        bool foundElephant = false;

        for (const auto& image : images) {
            if (image.title == "Elephant") {
                foundElephant = true;

                require(
                    image.descriptionPath ==
                        "database/descriptions/elephant.xml",
                    "Elephant's description path is incorrect."
                );

                std::cout << "Description path: "
                        << image.descriptionPath << "\n";
            }
        }

require(foundElephant, "Elephant record was not found.");
std::cout << "PASS: Elephant XML description path retrieved\n";

        auto games = database.findMedia("games", "food", 6);
        require(games.size() == 1, "Expected one food matching game.");
        std::cout << "PASS: Food matching game retrieved\n";

        auto sounds = database.findMedia("music", "nature", 2);
        require(sounds.size() == 3, "Expected three nature recordings.");

        for (const auto& sound : sounds) {
            require(sound.hasAudio, "Nature recordings must have audio.");
        }
        std::cout << "PASS: Three nature recordings with audio\n";

        auto videos = database.findMedia("video", "animals", 2);
        require(videos.size() == 2, "Expected two animal videos.");

        for (const auto& video : videos) {
            require(!video.hasAudio, "Animal videos should be silent.");
        }

        std::cout << "PASS: Two silent animal videos retrieved\n";

        require(
            database.findMedia("images", "animals", 13).empty(),
            "Expected no images outside the age range."
        );
        std::cout << "PASS: Age filter excludes age 13\n";

        require(
            database.findMedia("games", "food", 2).empty(),
            "Expected no matching game below its minimum age."
        );
        std::cout << "PASS: Game age filter excludes age 2\n";

        require(
            database.findMedia("video", "vehicles", 2).empty(),
            "Expected no videos for an unavailable topic."
        );

        auto instrumental = database.findMedia("music", "music", 2);
        require(instrumental.size() == 1, "Expected Water Lily.");
        require(instrumental.front().hasAudio,
                "Water Lily must have audio.");
        std::cout << "PASS: Instrumental audio retrieved\n";

        auto spaceVideos = database.findMedia("video", "space", 2);
        require(spaceVideos.size() == 1, "Expected Solar System.");
        require(spaceVideos.front().hasAudio,
                "Solar System must have an audio track.");
        std::cout << "PASS: Space video with audio retrieved\n";
        std::cout << "PASS: Unavailable topic returns no matches\n";

        std::cout << "\nAll ten database checks passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "Test failed: " << error.what() << "\n";
        return 1;
    }

    return 0;
}