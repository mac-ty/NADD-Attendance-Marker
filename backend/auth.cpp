#include "auth.h"
#include "storage.h"
#include "models.h"
#include "json.hpp"
#include "picosha2.h"
#include "storage.h"
#include <map>
#include <random>
using namespace std;

using json = nlohmann::json;

static map<string, string> loginTokens;

static string hashPassword(const string& password) {
    return picosha2::hash256_hex_string(password);
}

static string generateToken() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dist(0, 15);
    const char* hex = "0123456789abcdef";
    string token;

    for (int i = 0; i < 32; i++) {
        token += hex[dist(gen)];
    }

    return token;
}

void handleSignup(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        string name = body["name"];
        string email = body["email"];
        string password = body["password"];
        string repEmail = body["repEmail"];
        string courseCode = body["courseCode"];

        vector<Lecturer> lecturers = loadLecturers();
        for (const Lecturer& lecturer : lecturers) {
            if (lecturer.email == email) {
                res.status = 400;
                res.set_content(
                    json({
                        {"status", "error"},
                        {"reason", "email already registered"}
                    }).dump(),
                    "application/json"
                );
                return;
            }
        }

        Lecturer newLecturer;
        newLecturer.id = lecturers.empty() ? 1 : lecturers.back().id + 1;
        newLecturer.name = name;
        newLecturer.email = email;
        newLecturer.passwordHash = hashPassword(password);
        newLecturer.repEmail = repEmail;    
        newLecturer.courseCode = courseCode;

        appendLecturer(newLecturer);
        res.set_content(
            json({
                {"status", "success"}
            }).dump(),
            "application/json"
        );
    }
    catch (const exception& e) {
        res.status = 500;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", string("Server exception: ") + e.what()}
            }).dump(),
            "application/json"
        );
    }
}

void handleLogin(const httplib::Request& req, httplib::Response& res) {
    json body = json::parse(req.body);
    string email = body["email"];
    string password = body["password"];

    vector<Lecturer> lecturers = loadLecturers();
    string hashedInput = hashPassword(password);

    for (const Lecturer& lecturer : lecturers) {
        if (lecturer.email == email) {
            if (lecturer.passwordHash == hashedInput) {
                string token = generateToken();
                loginTokens[token] = email;
                res.set_content(
                json({
                    {"status", "success"},
                    {"token", token}
                }).dump(),
                "application/json");
            }
            else {
                res.status = 401;
                res.set_content(
                    json({
                        {"status", "error"},
                        {"reason", "incorrect password"}
                    }).dump(),
                    "application/json"
                );
            }
            return;
        }
    }

    res.status = 404;
    res.set_content(
        json({
            {"status", "error"},
            {"reason", "Account not found"}
        }).dump(),
        "application/json"
    );
}

bool isTokenValid(const string& token, string& outEmail) {
    auto it = loginTokens.find(token);

    if (it == loginTokens.end()) {
        return false;
    }

    outEmail = it -> second;
    return true;
}

void handleDebugLecturers(const httplib::Request& req, httplib::Response& res) {
    vector<Lecturer> lecturers = loadLecturers();
    json lecturerList = json::array();

    for (const Lecturer& lecturer : lecturers) {
        lecturerList.push_back({
            {"id", lecturer.id},
            {"name", lecturer.name},
            {"email", lecturer.email},
            {"repEmail", lecturer.repEmail},
            {"courseCode", lecturer.courseCode}
        });
    }

    res.set_content(
        json({
            {"status", "success"},
            {"lecturers", lecturerList}
        }).dump(),
        "application/json"
    );
}