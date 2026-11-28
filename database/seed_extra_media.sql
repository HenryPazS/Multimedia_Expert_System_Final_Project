BEGIN TRANSACTION;

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license, has_audio
)
VALUES
(
    'Gentle Rain', 'music', 'nature', 0, 12,
    'database/media/audio/nature/gentle-rain.wav',
    'rasunter255',
    'https://freesound.org/people/rasunter255/sounds/416227/',
    'CC0-1.0', 1
),
(
    'Water Lily', 'music', 'music', 0, 12,
    'database/media/audio/instrumental/water-lily.mp3',
    'Kevin MacLeod',
    'https://incompetech.com/music/royalty-free/index.html?isrc=USUAN1400035',
    'CC-BY-4.0', 1
),
(
    'Jellyfish', 'video', 'animals', 0, 12,
    'database/media/video/animals/jellyfish.mp4',
    'K',
    'https://www.pexels.com/video/close-up-view-of-a-jellyfish-2397239/',
    'Pexels License', 0
),
(
    'Forest Waterfall', 'video', 'nature', 0, 12,
    'database/media/video/nature/forest-waterfall.mp4',
    'K',
    'https://www.pexels.com/video/aerial-view-of-forest-with-flowing-waterfall-37019268/',
    'Pexels License', 0
),
(
    'Solar System', 'video', 'space', 0, 12,
    'database/media/video/space/solar-system.mp4',
    'Endiae Genius',
    'https://www.pexels.com/video/the-planets-of-the-solar-system-4980005/',
    'Pexels License', 1
)
ON CONFLICT(file_path) DO NOTHING;

COMMIT;