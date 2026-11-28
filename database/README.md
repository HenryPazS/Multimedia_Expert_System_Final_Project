# Multimedia Database

This module stores media records, retrieves activities by media type,
topic, and age, and reads MPEG-7-based XML descriptions.

## Requirements

The current build script supports macOS with Homebrew.

Required tools:
- C++17 compiler (`clang++`)
- SQLite command-line tool and development library
- Python 3.9 or newer
- TinyXML-2

Install TinyXML-2:

```bash
brew install tinyxml2
```

Run all commands below from the repository root.

## Media Files

The collection contains:
- 52 images: animals and space
- 4 audio items
- 4 videos
- 1 food memory-matching game

See [MEDIA_CREDITS.md](MEDIA_CREDITS.md) for creators, licenses,
and source links.

Download the four Pexels clips from those links and save them as:

```text
database/media/video/animals/aquarium-fish.mp4
database/media/video/animals/jellyfish.mp4
database/media/video/nature/forest-waterfall.mp4
database/media/video/space/solar-system.mp4
```

Use these exact paths. The downloaded videos are not included in
the repository. The full collection tests require them locally.

## Create the Database

```bash
bash database/setup_database.sh
```

This applies the schema and all seed files. Repeating setup does not
duplicate records. It does not download media or migrate older schemas.

Expected totals:

| Media type | Records |
|---|---:|
| images | 52 |
| music | 4 |
| video | 4 |
| games | 1 |

The backend uses `music` for both instrumental music and nature audio.

## Build and Run Tests

```bash
bash database/run_tests.sh
```

This compiles and runs:
- `test_database`: retrieval, age filtering, and audio flags
- `test_description`: XML reading, file links, and annotation searches
- `test_integration`: patient profile → inference recommendation →
  matching database records

The current integration example uses a child interested in space
with video as the preferred media type.

## Main Files

| File | Purpose |
|---|---|
| `DistractionMedia.h` | Holds one media record |
| `MediaDatabase.h/.cpp` | SQLite connection and retrieval |
| `MediaDescription.h/.cpp` | Reads supported XML descriptions |
| `schema.sql` | Database structure |
| `seed*.sql` | Media records and description links |
| `descriptions/` | XML description files |
| `media/` | Images, audio, video, and game files |

## Backend Integration

`findMedia(mediaType, category, age)` returns matching media records.

Supported media types:
- `images`
- `music`
- `video`
- `games`

Current topics include `animals`, `space`, `nature`, `music`, and `food`.

`findMediaByAnnotation(mediaType, category, age, searchText)`
further filters candidates using case-insensitive substring matching
against their XML annotations.

No matches produce an empty vector. Database and XML reading errors
produce exceptions. The session layer must handle these outcomes.

File paths stored in SQLite are relative to the repository root.
The frontend/backend must map those paths to served URLs; they are
not automatically browser-accessible URLs.

## Limitations

- Age ranges are provisional and need domain-expert review.
- Each record currently has one topic category.
- The caller must coordinate retrieval across multiple child interests.
- XML parsing supports the specific structures and namespace spelling
  used by this project.
- Full MPEG-7 schema validation has not been completed.
- Annotation searching is separate from the engine's default recommendation
  flow.
- Playback and physician-feedback integration belong to the frontend and
  consultation-session modules.
- Separate licensing for the upstream game's food graphics has not been
  independently verified.