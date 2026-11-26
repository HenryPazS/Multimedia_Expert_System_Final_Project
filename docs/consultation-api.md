# Proposed consultation API

The browser currently uses an in-memory demo service. The C++ team would need to implement these routes or an equivalent adapter.

| Request | JSON body | Success |
| --- | --- | --- |
| POST /api/consultations | `{"age":2,"interests":["animals","colors"],"preferences":["visuals","video","audio"]}` | 201 with session |
| POST /api/consultations/{id}/feedback | `{"value":"ineffective"}` | 200 with updated session |
| POST /api/consultations/{id}/end | Empty | 200 with ended session |

Sessions have a string `id`, an `active` or `ended` status, ordered recommendation `history`, and ordered `feedback` entries. Feedback entries include `value` and `previousCategory`. Valid values are `effective`, `ineffective`, and `disruptive`.

Each decision includes the C++ fields `recommendedMediaType`, `firedRules`, and `rationale`. Supported decisions are `images`, `video`, `music`, `games`, and `manual_intervention`. An optional `item` has `title`, `kind` (image, video, or audio), `url`, and `description`. Omit the item when the catalog has no match. An empty history represents no available recommendation. Return errors as JSON with a string `message` and an appropriate HTTP status.

Keep one inference engine per active consultation. Validate age, feedback, and canonical preferences at the API boundary, serialize feedback for each session, and snapshot every engine decision. Ending a session should prevent further feedback and release its engine state. Agree on catalog lookup and media hosting before connecting the browser. A manual category override would require a separate backend capability.
