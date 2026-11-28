#ifndef MEDIA_DATABASE_H
#define MEDIA_DATABASE_H

#include "DistractionMedia.h"
#include <sqlite3.h>
#include <string>
#include <vector>

class MediaDatabase {
private:
    sqlite3* database = nullptr;

public:
    explicit MediaDatabase(const std::string& databasePath);
    ~MediaDatabase();

    // Prevent copying the same database connection.
    MediaDatabase(const MediaDatabase&) = delete;
    MediaDatabase& operator=(const MediaDatabase&) = delete;

    std::vector<DistractionMedia> findMedia(
        const std::string& mediaType,
        const std::string& category,
        double age
    ) const;

    std::vector<DistractionMedia> findMediaByAnnotation(
        const std::string& mediaType,
        const std::string& category,
        double age,
        const std::string& searchText
    ) const;
};

#endif