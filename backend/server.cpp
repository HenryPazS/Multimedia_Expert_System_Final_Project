// HTTP API that connects the React frontend to the C++ backend.
//
//   POST   /api/sessions                  start a consultation (parent form)
//   GET    /api/sessions/:id              current activity + facts
//   POST   /api/sessions/:id/feedback     physician feedback -> next activity
//   DELETE /api/sessions/:id              end the consultation
//   GET    /api/health
//   GET    /media/...                     the files under database/media
//
// Run from the repository root:  server [--port 8080] [--db database/media.db] [--root .]

#include "ConsultationSession.h"
#include "MediaDatabase.h"
#include "PatientProfile.h"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

constexpr size_t kMaxSessions = 200;
constexpr size_t kMaxListItems = 20;

std::mutex sessionsMutex;  // guards `sessions` and every session in it
std::map<std::string, std::unique_ptr<ConsultationSession>> sessions;

std::string newSessionId() {
    static std::mt19937_64 rng{std::random_device{}()};
    static const char* hex = "0123456789abcdef";
    std::string id;
    for (int i = 0; i < 16; ++i) id += hex[rng() % 16];
    return id;
}

// database/media/images/a.png -> /media/images/a.png
std::string mediaUrl(const std::string& filePath) {
    const std::string prefix = "database/media/";
    if (filePath.rfind(prefix, 0) == 0) return "/media/" + filePath.substr(prefix.size());
    return "/" + filePath;
}

json mediaToJson(const DistractionMedia& m) {
    return {{"id", m.id},
            {"title", m.title},
            {"mediaType", m.mediaType},
            {"category", m.category},
            {"minAge", m.minAge},
            {"maxAge", m.maxAge},
            {"filePath", m.filePath},
            {"url", mediaUrl(m.filePath)},
            {"hasAudio", m.hasAudio},
            {"creator", m.creator},
            {"license", m.license},
            {"sourceUrl", m.sourceUrl},
            {"descriptionPath", m.descriptionPath}};
}

json sessionToJson(const ConsultationSession& s) {
    const ActivitySuggestion& a = s.getCurrentActivity();
    json media = json::array();
    for (const auto& m : a.media) media.push_back(mediaToJson(m));

    return {{"sessionId", s.getId()},
            {"activity",
             {{"mediaType", a.mediaType},
              {"manualIntervention", a.manualIntervention},
              {"firedRules", a.firedRules},
              {"rationale", a.rationale},
              {"media", media}}},
            {"facts", s.getFacts()},
            {"feedbackHistory", s.getFeedbackHistory()}};
}

void sendJson(httplib::Response& res, int status, const json& body) {
    res.status = status;
    res.set_content(body.dump(), "application/json");
}

void sendError(httplib::Response& res, int status, const std::string& message) {
    sendJson(res, status, {{"error", message}});
}

std::vector<std::string> readStringList(const json& body, const char* key) {
    std::vector<std::string> out;
    if (!body.contains(key)) return out;
    const json& list = body.at(key);
    if (!list.is_array()) throw std::invalid_argument(std::string("'") + key + "' must be an array of strings.");
    if (list.size() > kMaxListItems) throw std::invalid_argument(std::string("'") + key + "' has too many items.");
    for (const auto& item : list) {
        if (!item.is_string()) throw std::invalid_argument(std::string("'") + key + "' must be an array of strings.");
        out.push_back(item.get<std::string>());
    }
    return out;
}

// Wraps a handler so bad input becomes a 400 and unexpected failures a 500.
template <typename Fn>
void guarded(httplib::Response& res, Fn&& fn) {
    try {
        fn();
    } catch (const json::exception& e) {
        sendError(res, 400, std::string("Invalid JSON: ") + e.what());
    } catch (const std::invalid_argument& e) {
        sendError(res, 400, e.what());
    } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << "\n";
        sendError(res, 500, "Internal server error.");
    }
}

}  // namespace

int main(int argc, char** argv) {
    int port = 8080;
    std::string dbPath = "database/media.db";
    std::string root = ".";

    for (int i = 1; i + 1 < argc; i += 2) {
        std::string flag = argv[i];
        if (flag == "--port") port = std::atoi(argv[i + 1]);
        else if (flag == "--db") dbPath = argv[i + 1];
        else if (flag == "--root") root = argv[i + 1];
        else {
            std::cerr << "Unknown option " << flag << "\n";
            return 2;
        }
    }

    std::shared_ptr<const MediaDatabase> database;
    try {
        database = std::make_shared<MediaDatabase>(dbPath);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\nRun bash database/setup_database.sh first, and start the server "
                     "from the repository root (or pass --db and --root).\n";
        return 1;
    }

    httplib::Server server;

    // The React dev server runs on another origin, so allow cross-origin calls.
    server.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });
    server.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) { res.status = 204; });

    if (!server.set_mount_point("/media", root + "/database/media")) {
        std::cerr << "Warning: media folder not found at " << root << "/database/media\n";
    }

    server.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        sendJson(res, 200, {{"status", "ok"}});
    });

    // Parent form -> new consultation + first recommendation.
    server.Post("/api/sessions", [&](const httplib::Request& req, httplib::Response& res) {
        guarded(res, [&] {
            json body = json::parse(req.body);
            if (!body.is_object() || !body.contains("age") || !body["age"].is_number()) {
                throw std::invalid_argument("'age' is required and must be a number.");
            }
            double age = body["age"].get<double>();
            if (!std::isfinite(age) || age < 0 || age > 18) {
                throw std::invalid_argument("'age' must be between 0 and 18.");
            }
            auto interests = readStringList(body, "interests");
            auto preferences = readStringList(body, "preferences");
            if (preferences.empty()) {
                throw std::invalid_argument("'preferences' needs at least one media preference.");
            }

            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.size() >= kMaxSessions) {
                sendError(res, 503, "Too many active sessions.");
                return;
            }
            std::string id = newSessionId();
            auto session = std::make_unique<ConsultationSession>(
                id, PatientProfile(age, interests, preferences), database, root);
            json out = sessionToJson(*session);
            sessions[id] = std::move(session);
            sendJson(res, 201, out);
        });
    });

    server.Get(R"(/api/sessions/([0-9a-f]+))", [&](const httplib::Request& req, httplib::Response& res) {
        guarded(res, [&] {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            auto it = sessions.find(req.matches[1]);
            if (it == sessions.end()) return sendError(res, 404, "Session not found.");
            sendJson(res, 200, sessionToJson(*it->second));
        });
    });

    // Physician feedback -> next activity.
    server.Post(R"(/api/sessions/([0-9a-f]+)/feedback)", [&](const httplib::Request& req, httplib::Response& res) {
        guarded(res, [&] {
            json body = json::parse(req.body);
            if (!body.is_object() || !body.contains("feedback") || !body["feedback"].is_string()) {
                throw std::invalid_argument("'feedback' is required: effective, ineffective or disruptive.");
            }
            std::lock_guard<std::mutex> lock(sessionsMutex);
            auto it = sessions.find(req.matches[1]);
            if (it == sessions.end()) return sendError(res, 404, "Session not found.");
            it->second->recordPhysicianFeedback(body["feedback"].get<std::string>());
            sendJson(res, 200, sessionToJson(*it->second));
        });
    });

    server.Delete(R"(/api/sessions/([0-9a-f]+))", [&](const httplib::Request& req, httplib::Response& res) {
        guarded(res, [&] {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.erase(req.matches[1]) == 0) return sendError(res, 404, "Session not found.");
            res.status = 204;
        });
    });

    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        if (res.body.empty()) sendError(res, res.status, "Request failed.");
    });

    std::cout << "Server listening on http://localhost:" << port << "\n";
    if (!server.listen("127.0.0.1", port)) {
        std::cerr << "Could not listen on port " << port << "\n";
        return 1;
    }
    return 0;
}
