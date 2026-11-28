#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

echo "Applying database schema..."
sqlite3 -bail database/media.db < database/schema.sql

echo "Adding original media..."
sqlite3 -bail database/media.db < database/seed.sql

echo "Adding space images..."
sqlite3 -bail database/media.db < database/seed_space.sql

echo "Adding extra audio and videos..."
sqlite3 -bail database/media.db < database/seed_extra_media.sql

echo "Linking extra descriptions..."
sqlite3 -bail database/media.db < database/seed_extra_descriptions.sql

echo
sqlite3 -header -column database/media.db \
    "SELECT media_type, COUNT(*) AS total
     FROM media
     GROUP BY media_type;"

echo
echo "Database setup completed."