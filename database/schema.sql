CREATE TABLE IF NOT EXISTS media (
    id INTEGER PRIMARY KEY,
    title TEXT NOT NULL,
    media_type TEXT NOT NULL
        CHECK (media_type IN ('images', 'music', 'video', 'games')),
    category TEXT NOT NULL,
    min_age REAL NOT NULL CHECK (min_age >= 0),
    max_age REAL NOT NULL CHECK (max_age >= min_age),
    file_path TEXT NOT NULL UNIQUE,
    creator TEXT NOT NULL,
    source_url TEXT NOT NULL,
    license TEXT NOT NULL,
    has_audio INTEGER NOT NULL DEFAULT 0
        CHECK (has_audio IN (0, 1)),
    description_path TEXT
);