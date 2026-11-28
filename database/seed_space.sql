BEGIN TRANSACTION;

INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Black Spacecraft Design 1', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlack1.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlack1.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Black Spacecraft Design 2', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlack2.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlack2.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Black Spacecraft Design 3', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlack3.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlack3.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Black Spacecraft Design 4', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlack4.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlack4.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Black Spacecraft Design 5', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlack5.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlack5.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spacecraft Design 1', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlue1.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlue1.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spacecraft Design 2', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlue2.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlue2.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spacecraft Design 3', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlue3.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlue3.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spacecraft Design 4', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlue4.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlue4.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spacecraft Design 5', 'images', 'space', 0, 12,
    'database/media/images/space/enemyBlue5.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyBlue5.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spacecraft Design 1', 'images', 'space', 0, 12,
    'database/media/images/space/enemyGreen1.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyGreen1.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spacecraft Design 2', 'images', 'space', 0, 12,
    'database/media/images/space/enemyGreen2.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyGreen2.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spacecraft Design 3', 'images', 'space', 0, 12,
    'database/media/images/space/enemyGreen3.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyGreen3.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spacecraft Design 4', 'images', 'space', 0, 12,
    'database/media/images/space/enemyGreen4.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyGreen4.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spacecraft Design 5', 'images', 'space', 0, 12,
    'database/media/images/space/enemyGreen5.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyGreen5.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spacecraft Design 1', 'images', 'space', 0, 12,
    'database/media/images/space/enemyRed1.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyRed1.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spacecraft Design 2', 'images', 'space', 0, 12,
    'database/media/images/space/enemyRed2.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyRed2.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spacecraft Design 3', 'images', 'space', 0, 12,
    'database/media/images/space/enemyRed3.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyRed3.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spacecraft Design 4', 'images', 'space', 0, 12,
    'database/media/images/space/enemyRed4.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyRed4.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spacecraft Design 5', 'images', 'space', 0, 12,
    'database/media/images/space/enemyRed5.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/enemyRed5.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 00', 'images', 'space', 0, 12,
    'database/media/images/space/planet00.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet00.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 01', 'images', 'space', 0, 12,
    'database/media/images/space/planet01.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet01.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 02', 'images', 'space', 0, 12,
    'database/media/images/space/planet02.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet02.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 03', 'images', 'space', 0, 12,
    'database/media/images/space/planet03.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet03.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 04', 'images', 'space', 0, 12,
    'database/media/images/space/planet04.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet04.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 05', 'images', 'space', 0, 12,
    'database/media/images/space/planet05.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet05.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 06', 'images', 'space', 0, 12,
    'database/media/images/space/planet06.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet06.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 07', 'images', 'space', 0, 12,
    'database/media/images/space/planet07.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet07.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 08', 'images', 'space', 0, 12,
    'database/media/images/space/planet08.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet08.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Fantasy Planet 09', 'images', 'space', 0, 12,
    'database/media/images/space/planet09.png', 'Kenney', 'https://kenney.nl/assets/planets', 'CC0-1.0',
    0, 'database/descriptions/space/planet09.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spaceship Style 1', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip1_blue.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip1_blue.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spaceship Style 1', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip1_green.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip1_green.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Orange Spaceship Style 1', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip1_orange.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip1_orange.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spaceship Style 1', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip1_red.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip1_red.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spaceship Style 2', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip2_blue.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip2_blue.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spaceship Style 2', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip2_green.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip2_green.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Orange Spaceship Style 2', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip2_orange.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip2_orange.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spaceship Style 2', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip2_red.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip2_red.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Blue Spaceship Style 3', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip3_blue.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip3_blue.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Green Spaceship Style 3', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip3_green.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip3_green.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Orange Spaceship Style 3', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip3_orange.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip3_orange.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;


INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    'Red Spaceship Style 3', 'images', 'space', 0, 12,
    'database/media/images/space/playerShip3_red.png', 'Kenney', 'https://kenney.nl/assets/space-shooter-remastered', 'CC0-1.0',
    0, 'database/descriptions/space/playerShip3_red.xml'
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;

COMMIT;