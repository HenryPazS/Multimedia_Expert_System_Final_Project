# Adaptive Child Distraction frontend

From this directory, use Node 20.19+ on the Node 20 line or Node 22.12+ on a supported newer line.

```sh
npm ci
npm run dev
npm run build
npm run lint
npm run test
```

Open the URL printed by Vite. The default application uses an in-memory service and labels its recommendations as simulated. Refreshing clears the consultation. Enter sample information only.

## Source layout

| Location | Responsibility |
| --- | --- |
| `src/App.tsx` | Chooses the screen, holds the session, and manages requests and errors |
| `src/pages/ParentSetup.tsx` | Collects age and comma-separated interests, validates input, and ranks selected media |
| `src/pages/PhysicianDashboard.tsx` | Shows the current decision, explanation, fired rules, most recent feedback, and history |
| `src/components/MediaPreview.tsx` | Displays an image or gives playback controls for video and audio. Handles missing media and load failures |
| `src/types/consultation.ts` | Defines profile, feedback, media, decision, and session types |
| `src/services/` | Defines the service, validates input, and provides mock and proposed HTTP adapters |
| `src/data/demo.ts`, `public/demo/` | Recorded sample decision fixtures and locally generated media |

Ages can be fractional and must be finite and nonnegative. Preferences use the C++ input vocabulary. The demo uses preference order for general inputs. The age-2 sample with images, video, and audio in that order includes rule explanations based on the C++ console demo. The browser has no fuzzy inference engine. General demo decisions do not show C++ rule identifiers.

The proposed HTTP adapter is exercised with mocked responses and can be passed into `App` after a compatible API is available. The default entry point remains in demo mode. Integration requires server-side session management and a multimedia catalog. Manual category selection requires a backend capability. Games can be recommended, but the local catalog has no playable game. The demo image, silent video, and synthesized audio ship with the frontend and require no external URLs.

- [Proposed API contract](../docs/consultation-api.md)
- [QA and demo instructions](../docs/frontend-qa-demo.md)
- [Final report contribution](../docs/frontend-report.md)
