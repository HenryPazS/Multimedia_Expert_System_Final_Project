BEGIN TRANSACTION;
UPDATE media SET description_path = 'database/descriptions/gentle-rain.xml' WHERE file_path = 'database/media/audio/nature/gentle-rain.wav';
UPDATE media SET description_path = 'database/descriptions/water-lily.xml' WHERE file_path = 'database/media/audio/instrumental/water-lily.mp3';
UPDATE media SET description_path = 'database/descriptions/jellyfish.xml' WHERE file_path = 'database/media/video/animals/jellyfish.mp4';
UPDATE media SET description_path = 'database/descriptions/forest-waterfall.xml' WHERE file_path = 'database/media/video/nature/forest-waterfall.mp4';
UPDATE media SET description_path = 'database/descriptions/solar-system.xml' WHERE file_path = 'database/media/video/space/solar-system.mp4';
COMMIT;