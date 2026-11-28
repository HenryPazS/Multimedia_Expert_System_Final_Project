INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license
)
VALUES (
    'Elephant',
    'images',
    'animals',
    0,
    12,
    'database/media/images/animals/elephant.png',
    'Kenney Vleugels',
    'https://kenney.nl/assets/animal-pack',
    'CC0-1.0'
)
ON CONFLICT(file_path) DO NOTHING;

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license
)
VALUES
('Giraffe', 'images', 'animals', 0, 12,
 'database/media/images/animals/giraffe.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Hippo', 'images', 'animals', 0, 12,
 'database/media/images/animals/hippo.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Monkey', 'images', 'animals', 0, 12,
 'database/media/images/animals/monkey.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Panda', 'images', 'animals', 0, 12,
 'database/media/images/animals/panda.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Parrot', 'images', 'animals', 0, 12,
 'database/media/images/animals/parrot.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Penguin', 'images', 'animals', 0, 12,
 'database/media/images/animals/penguin.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Pig', 'images', 'animals', 0, 12,
 'database/media/images/animals/pig.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Rabbit', 'images', 'animals', 0, 12,
 'database/media/images/animals/rabbit.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0'),

('Snake', 'images', 'animals', 0, 12,
 'database/media/images/animals/snake.png',
 'Kenney Vleugels', 'https://kenney.nl/assets/animal-pack', 'CC0-1.0')
ON CONFLICT(file_path) DO NOTHING;

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license
)
VALUES (
    'Food Memory Matching Game',
    'games',
    'food',
    4,
    12,
    'database/media/games/animal-match/index.html',
    'Ania Kubow',
    'https://github.com/kubowania/memory-game',
    'MIT'
)
ON CONFLICT(file_path) DO NOTHING;

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license
)
VALUES
(
    'Gentle Waves',
    'music',
    'nature',
    0, 12,
    'database/media/audio/nature/gentle-waves.wav',
    'Alex_hears_things',
    'https://freesound.org/people/Alex_hears_things/sounds/352356/',
    'CC0-1.0'
),
(
    'Bird Sounds',
    'music',
    'nature',
    0, 12,
    'database/media/audio/nature/birds.wav',
    'Sadiquecat',
    'https://freesound.org/people/Sadiquecat/sounds/802704/',
    'CC0-1.0'
)
ON CONFLICT(file_path) DO NOTHING;

UPDATE media
SET has_audio = 1
WHERE file_path IN (
    'database/media/audio/nature/gentle-waves.wav',
    'database/media/audio/nature/birds.wav'
);

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license, has_audio
)
VALUES (
    'Aquarium Fish',
    'video',
    'animals',
    0, 12,
    'database/media/video/animals/aquarium-fish.mp4',
    'Magda Ehlers',
    'https://www.pexels.com/video/colorful-fish-swimming-in-aquarium-34945018/',
    'Pexels License',
    0
)
ON CONFLICT(file_path) DO NOTHING;

UPDATE media
SET description_path = 'database/descriptions/elephant.xml'
WHERE file_path = 'database/media/images/animals/elephant.png';

UPDATE media
SET description_path = 'database/descriptions/gentle-waves.xml'
WHERE file_path = 'database/media/audio/nature/gentle-waves.wav';

UPDATE media
SET description_path = 'database/descriptions/aquarium-fish.xml'
WHERE file_path = 'database/media/video/animals/aquarium-fish.mp4';

UPDATE media
SET description_path = 'database/descriptions/birds.xml'
WHERE file_path = 'database/media/audio/nature/birds.wav';

UPDATE media
SET description_path =
    'database/descriptions/' ||
    lower(title) ||
    '.xml'
WHERE media_type = 'images'
  AND category = 'animals'
  AND title IN (
      'Elephant', 'Giraffe', 'Hippo', 'Monkey', 'Panda',
      'Parrot', 'Penguin', 'Pig', 'Rabbit', 'Snake'
  );

UPDATE media
SET description_path = 'database/descriptions/food-matching.xml'
WHERE file_path = 'database/media/games/animal-match/index.html';