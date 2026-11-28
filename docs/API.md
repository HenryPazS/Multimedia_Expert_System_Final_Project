# Backend API (C++ server -> React frontend)

Build, then start from the **repository root** (the database and media paths are relative to it):

```bash
bash database/setup_database.sh            # once
cmake -S . -B build/cmake && cmake --build build/cmake --config Release
build/cmake/server --port 8080             # options: --db <path> --root <repo root>
```

All bodies are JSON. CORS is open, so the React dev server can call it directly.

## Endpoints

| Method | Path | Purpose |
|---|---|---|
| POST | `/api/sessions` | Parent form -> start a consultation, returns the first activity (201) |
| GET | `/api/sessions/:id` | Current activity |
| POST | `/api/sessions/:id/feedback` | Physician feedback -> next activity |
| DELETE | `/api/sessions/:id` | End the consultation (204) |
| GET | `/api/health` | `{"status":"ok"}` |
| GET | `/media/...` | Static media files (`database/media/...`) |

### Start a session

```json
POST /api/sessions
{ "age": 2, "interests": ["space"], "preferences": ["visuals", "video", "audio"] }
```

`preferences` is in the parent's order of preference (`visuals`/`images`, `video`, `audio`/`music`,
`interaction`/`games`). `interests` are library topics: `animals`, `space`, `nature`, `music`, `food`
(other words are accepted but fall back to any topic). Age must be 0-18.

### Send feedback

```json
POST /api/sessions/:id/feedback
{ "feedback": "ineffective" }      // "effective" | "ineffective" | "disruptive"
```

### Response (both endpoints)

```json
{
  "sessionId": "9f2c...",
  "activity": {
    "mediaType": "images",            // images | music | video | games | manual_intervention
    "manualIntervention": false,      // true = Rule 10, the physician takes over; media is empty
    "firedRules": ["Rule 1"],
    "rationale": "Child is very young ...",
    "media": [                        // media[0] is the one to show; the rest are alternatives
      { "id": 15, "title": "...", "mediaType": "images", "category": "space",
        "url": "/media/images/space/enemyBlack1.png", "hasAudio": false,
        "creator": "Kenney", "license": "CC0-1.0", "sourceUrl": "...",
        "descriptionPath": "database/descriptions/space/enemyBlack1.xml" }
    ]
  },
  "facts": ["Age=2 (...)", "ParentPref=visuals", "Recommended=images"],
  "feedbackHistory": ["ineffective"]
}
```

Prefix `url` with the server address (e.g. `http://localhost:8080/media/...`). Games are an
`index.html` page: show it in an `<iframe>`. Images/music/video use `<img>`/`<audio>`/`<video>`.

Errors are `{"error": "..."}` with status 400 (bad input), 404 (unknown session), 503 (too many sessions).

## Fallback behaviour

- **Disruptive video** -> removed for the session, music suggested (Rules 3-4).
- **Disruptive game** -> removed, music suggested, or images if music is unavailable (Rules 5-6).
- **Ineffective** -> next preference from the parent's list; a type already judged ineffective is not offered again (Rule 8).
- **Effective** -> keep the same activity (Rule 7).
- **Nothing left** (everything preferred was ineffective/disruptive, or the library has no playable file for the child's age) -> `manual_intervention` (Rule 10).
- Media whose file is missing on the server (e.g. the separately downloaded videos) is never offered.
- Feedback sent while nothing is playing (e.g. after `manual_intervention`) is ignored.
