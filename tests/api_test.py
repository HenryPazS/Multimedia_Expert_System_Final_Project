"""End-to-end check of the HTTP API. Needs only Python 3.

Run from the repository root:  python tests/api_test.py <path to server executable>
Starts the real server on a free port, plays through the proposal's example
consultation plus the fallback cases, and stops the server.
"""
import json
import socket
import subprocess
import sys
import time
import urllib.error
import urllib.request

failures = 0


def check(ok, what):
    global failures
    print(("ok   " if ok else "FAIL ") + what)
    if not ok:
        failures += 1


def call(base, method, path, body=None):
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(base + path, data=data, method=method,
                                 headers={"Content-Type": "application/json"})
    try:
        with urllib.request.urlopen(req) as r:
            raw = r.read()
            return r.status, (json.loads(raw) if raw and "json" in r.headers.get("Content-Type", "") else raw)
    except urllib.error.HTTPError as e:
        return e.code, json.loads(e.read() or b"{}")


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)

    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        port = s.getsockname()[1]
    server = subprocess.Popen([sys.argv[1], "--port", str(port)])
    base = f"http://127.0.0.1:{port}"

    try:
        for _ in range(50):
            try:
                if call(base, "GET", "/api/health")[0] == 200:
                    break
            except OSError:
                time.sleep(0.1)
        else:
            sys.exit("server did not start (is database/media.db built?)")

        # Proposal example: images -> ineffective -> next preference -> disruptive -> music.
        code, st = call(base, "POST", "/api/sessions",
                        {"age": 2, "interests": ["space"], "preferences": ["visuals", "video", "audio"]})
        sid = st["sessionId"]
        check(code == 201 and st["activity"]["mediaType"] == "images", "start: very young + visuals -> images")
        check(st["activity"]["media"] and st["activity"]["media"][0]["url"].startswith("/media/"),
              "media items carry a /media URL")
        code, body = call(base, "GET", st["activity"]["media"][0]["url"])
        check(code == 200, "media file is served")

        _, st = call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "ineffective"})
        first = st["activity"]["mediaType"]
        check(first != "images" and not st["activity"]["manualIntervention"],
              f"ineffective -> moves on ({first})")

        _, st = call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "effective"})
        check(st["activity"]["mediaType"] == first, "effective -> same activity kept")

        _, st = call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "disruptive"})
        check(st["activity"]["mediaType"] != first, "disruptive -> activity removed")
        for _ in range(5):  # keep calling it disruptive until the options run out
            if st["activity"]["manualIntervention"]:
                break
            _, st = call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "disruptive"})
        check(st["activity"]["manualIntervention"] and not st["activity"]["media"],
              "options exhausted -> manual intervention")

        code, again = call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "effective"})
        check(code == 200 and again["activity"]["manualIntervention"],
              "feedback after manual intervention is ignored")

        # Errors.
        check(call(base, "POST", f"/api/sessions/{sid}/feedback", {"feedback": "great"})[0] == 400, "bad feedback -> 400")
        check(call(base, "POST", "/api/sessions", {"age": -1, "preferences": ["video"]})[0] == 400, "bad age -> 400")
        check(call(base, "POST", "/api/sessions", {"age": 5})[0] == 400, "missing preferences -> 400")
        check(call(base, "GET", "/api/sessions/deadbeef")[0] == 404, "unknown session -> 404")

        # A game for an older child; disruptive game -> music (Rules 5-6), not video.
        _, st = call(base, "POST", "/api/sessions",
                     {"age": 10, "interests": ["food"], "preferences": ["interaction", "video"]})
        check(st["activity"]["mediaType"] == "games", "older child + interaction -> games")
        _, st = call(base, "POST", f"/api/sessions/{st['sessionId']}/feedback", {"feedback": "disruptive"})
        check(st["activity"]["mediaType"] in ("music", "images"), "disruptive game -> music or images")

        check(call(base, "DELETE", f"/api/sessions/{sid}")[0] == 204, "session can be deleted")
        check(call(base, "GET", f"/api/sessions/{sid}")[0] == 404, "deleted session is gone")
    finally:
        server.terminate()
        server.wait()

    print("All API checks passed." if not failures else f"{failures} check(s) failed.")
    sys.exit(1 if failures else 0)


main()
