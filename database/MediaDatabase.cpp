#include "MediaDatabase.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include "MediaDescription.h"
#include <algorithm>
#include <cctype>

MediaDatabase::MediaDatabase(const std::string& databasePath) {
    int result = sqlite3_open_v2(
        databasePath.c_str(),
        &database,
        SQLITE_OPEN_READONLY,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::string message = database
            ? sqlite3_errmsg(database)
            : "Could not allocate a database connection.";

        if (database) {
            sqlite3_close(database);
            database = nullptr;
        }

        throw std::runtime_error("Cannot open database: " + message);
    }
}

MediaDatabase::~MediaDatabase() {
    if (database) {
        sqlite3_close(database);
    }
}

std::vector<DistractionMedia> MediaDatabase::findMedia(
    const std::string& mediaType,
    const std::string& category,
    double age
) const {
    if (!std::isfinite(age) || age < 0) {
        throw std::invalid_argument("Age must be a nonnegative number.");
    }

    const char* query = R"(
        SELECT id, title, media_type, category, min_age, max_age,
            file_path, creator, source_url, license, has_audio,
            description_path
        FROM media
        WHERE media_type = ?
        AND category = ?
        AND ? BETWEEN min_age AND max_age
        ORDER BY id;
    )";

    sqlite3_stmt* rawStatement = nullptr;

    int result = sqlite3_prepare_v2(
        database, query, -1, &rawStatement, nullptr
    );

    // Automatically release the statement, including after an error.
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>
        statement(rawStatement, sqlite3_finalize);

    if (result != SQLITE_OK) {
        throw std::runtime_error(
            std::string("Cannot prepare query: ") +
            sqlite3_errmsg(database)
        );
    }

    auto checkBinding = [&](int status) {
        if (status != SQLITE_OK) {
            throw std::runtime_error(
                std::string("Cannot bind query value: ") +
                sqlite3_errmsg(database)
            );
        }
    };

    checkBinding(sqlite3_bind_text(
        statement.get(), 1, mediaType.c_str(), -1, SQLITE_TRANSIENT
    ));

    checkBinding(sqlite3_bind_text(
        statement.get(), 2, category.c_str(), -1, SQLITE_TRANSIENT
    ));

    checkBinding(sqlite3_bind_double(statement.get(), 3, age));

    auto readText = [&](int column) -> std::string {
        const unsigned char* text =
            sqlite3_column_text(statement.get(), column);

        return text ? reinterpret_cast<const char*>(text) : "";
    };

    std::vector<DistractionMedia> matches;

    while ((result = sqlite3_step(statement.get())) == SQLITE_ROW) {
        DistractionMedia media;

        media.id = sqlite3_column_int(statement.get(), 0);
        media.title = readText(1);
        media.mediaType = readText(2);
        media.category = readText(3);
        media.minAge = sqlite3_column_double(statement.get(), 4);
        media.maxAge = sqlite3_column_double(statement.get(), 5);
        media.filePath = readText(6);
        media.creator = readText(7);
        media.sourceUrl = readText(8);
        media.license = readText(9);
        media.hasAudio = sqlite3_column_int(statement.get(), 10) != 0;
        media.descriptionPath = readText(11);

        matches.push_back(media);
    }

    if (result != SQLITE_DONE) {
        throw std::runtime_error(
            std::string("Cannot complete query: ") +
            sqlite3_errmsg(database)
        );
    }

    return matches;
}

std::vector<DistractionMedia> MediaDatabase::findMediaByAnnotation(
    const std::string& mediaType,
    const std::string& category,
    double age,
    const std::string& searchText
) const {
    auto lowercase = [](std::string text) {
        std::transform(
            text.begin(), text.end(), text.begin(),
            [](unsigned char character) {
                return static_cast<char>(std::tolower(character));
            }
        );
        return text;
    };

    if (searchText.empty()) {
        throw std::invalid_argument(
            "Annotation search text must not be empty."
        );
    }

    auto candidates = findMedia(mediaType, category, age);
    std::vector<DistractionMedia> matches;

    const std::string query = lowercase(searchText);

    for (const auto& media : candidates) {
        if (media.descriptionPath.empty()) {
            continue;
        }

        auto description =
            readMediaDescription(media.descriptionPath);

        if (lowercase(description.annotation).find(query) !=
            std::string::npos) {
            matches.push_back(media);
        }
    }

    return matches;
}