# Frontend QA and demo

## Verification results

| Check | Result |
| --- | --- |
| `npm run test` | 14 passed across 2 Vitest files |
| `npm run build` | Passed |
| `npm run lint` | Passed |
| Vite dev server | Started on port 5174 because Vite reported an existing process for the project on port 5173. The app and all three demo files returned HTTP 200 |
| Chrome demo rehearsal | Verified the desktop dashboard and the dashboard at 768 CSS pixels in Chrome’s responsive toolbar. The sample displayed the image, played the five-second video and four-second audio after pressing Play, and retained the complete history after the consultation ended |

## Test coverage

| Area | Cases |
| --- | --- |
| Parent setup | Missing age, interests, and preferences. Negative age. Fractional age, media selection, reordering, and successful submission |
| Physician dashboard | Initial recommendation, rationale, fired rules, feedback, updated media, history, and ending the session |
| Simulated scenario | Images → video → music, effective feedback, depleted preferences, missing media, and unsupported input |
| Media | Missing and failed items, video and audio require explicit playback |
| Proposed HTTP contract | Start and end requests, empty history, invalid response shapes, server and network failures, failed feedback, and retry |

### C++ console baseline

Run from the repository root.

```sh
c++ -std=c++17 backend/main.cpp backend/PatientProfile.cpp backend/InferenceEngine.cpp -o /tmp/consultation-demo
/tmp/consultation-demo
```

