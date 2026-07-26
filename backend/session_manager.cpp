#include "session_manager.h"
#include "auth.h"
#include "storage.h"
#include "json.hpp"
#include "qrcodegen.hpp"
#include "email_sender.h"
#include <map>
#include <iostream>
#include <sstream>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
using namespace std;
using json = nlohmann::json;
using qrcodegen::QrCode;

static map<string, Session> sessions;

static string generateSessionCode() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dist(0, 25);
    string sessionCode;

    for (int i = 0; i < 6; i++) {
        sessionCode += char('A' + dist(gen));
    }
    return sessionCode;
}

static string formatFilename(const string& input) {
    string result = input;
    for (char& c : result) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':') {
            c = '_';
        }
    }

    return result;
}

static string toSvgString(const QrCode& qr, int border) {
    ostringstream sb;
    int size = qr.getSize() + border * 2;

    sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 "
       << size << " " << size << "\">\n";
    sb << "<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    sb << "<path d=\"";

    for (int y = 0; y < qr.getSize(); y++) {
        for (int x = 0; x < qr.getSize(); x++) {
            if (qr.getModule(x, y)) {
                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z ";
            }
        }
    }

    sb << "\" fill=\"#000000\"/>\n</svg>\n";
    return sb.str();
}


Session* findSession(const string& sessionCode) {
    auto it = sessions.find(sessionCode);
    if (it == sessions.end()) {
        return nullptr;
    }
    return &(it -> second);
}

void handleStartSession(const httplib::Request& req, httplib::Response& res) {
    json body = json::parse(req.body);
    string token = body["token"];
    string sessionType = body["sessionType"];

    string lecturerEmail;
    if (!isTokenValid(token, lecturerEmail)) {
        res.status = 401;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "invalid token"}
            }).dump(),
            "application/json"
        );
        return;
    }

    vector<Lecturer> lecturers = loadLecturers();
    int lecturerID = -1;
    string courseCode;
    string repEmail;
    for (const Lecturer& lecturer : lecturers) {
        if (lecturer.email == lecturerEmail) {
            lecturerID = lecturer.id;
            courseCode = lecturer.courseCode;
            repEmail = lecturer.repEmail;
            break;
        }
    }

    Session newSession;
    newSession.sessionCode = generateSessionCode();
    newSession.lecturerID = lecturerID;
    newSession.sessionType = sessionType;
    newSession.courseCode = courseCode;
    newSession.startTime = time(nullptr);
    newSession.lecturerEmail = lecturerEmail;
    newSession.repEmail = repEmail;
    newSession.sendToLecturer = body.value("sendToLecturer", false);

    time_t now = time(nullptr);
    char dateBuffer[11];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", localtime(&now));
    newSession.csvFilename = formatFilename(courseCode) + "_" + string(dateBuffer) + ".csv";
    createSessionFile(newSession);

    if (sessionType == "face_to_face") {
        newSession.centerLat = body["lat"];
        newSession.centerLng = body["lng"];
    }

    sessions[newSession.sessionCode] = newSession;

    string scanURL = "https://nadd-attendance-marker.vercel.app/scan.html?session=" + newSession.sessionCode;

    QrCode qr = QrCode::encodeText(scanURL.c_str(), QrCode::Ecc::MEDIUM);
    string svg = toSvgString(qr, 4);

    res.set_content(
        json({
            {"status", "success"},
            {"sessionCode", newSession.sessionCode},
            {"qrSvg", svg}
        }).dump(),
        "application/json"
    );
}


void handleSessionStatus(const httplib::Request& req, httplib::Response& res) {
    string sessionCode = req.matches[1];
    Session* session = findSession(sessionCode);
    
    if (session == nullptr) {
        res.status = 404;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "session not found"}
            }).dump(),
            "application/json");
        return;
    }

    time_t now = time(nullptr);
    int elapsed = (int)difftime(now, session -> startTime);
    int secondsRemaining = session -> durationSeconds - elapsed;
    if (secondsRemaining < 0) secondsRemaining = 0;
    if (secondsRemaining == 0) session -> closed = true;

    if (session->closed && !session->emailSent) {
        sendAttendanceReport(*session);
        session->emailSent = true;
    }

    res.set_content(
        json({
            {"secondsRemaining", secondsRemaining},
            {"markedCount", (int)session -> marked.size()},
            {"closed", session -> closed},
            {"sessionType", session -> sessionType}
        }).dump(),
        "application/json"
    );
}


void handleCloseSession(const httplib::Request& req, httplib::Response& res) {
    string sessionCode = req.matches[1];

    Session* session = findSession(sessionCode);

    if (session == nullptr) {
        res.status = 404;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "session not found"}
            }).dump(),
            "application/json");
        return;
    }

    session -> closed = true;

    if (!session->emailSent) {
        sendAttendanceReport(*session);
        session->emailSent = true;
    }

    res.set_content(
        json({
            {"status", "success"},
            {"markedCount", (int)session -> marked.size()}
        }).dump(),
        "application/json");
}


void handleDebugSessionFile(const httplib::Request& req, httplib::Response& res) {
    string sessionCode = req.matches[1];
    Session* session = findSession(sessionCode);

    if (session == nullptr) {
        res.status = 404;
        res.set_content("session not found", "text/plain");
        return;
    }

    ifstream file("./database/sessions/" + session->csvFilename);
    if (!file.is_open()) {
        res.status = 404;
        res.set_content("csv file not found on disk", "text/plain");
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    res.set_content(buffer.str(), "text/plain");
}