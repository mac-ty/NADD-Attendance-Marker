#include "attendance.h"
#include "auth.h"
#include "session_manager.h"
#include "location.h"
#include "json.hpp"
#include <ctime>

using namespace std;
using json = nlohmann::json;


static string currentTimeString() {
    time_t now = time(nullptr);
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
    return string(buffer);
}


void handleMarkAttendance(const httplib::Request& req, httplib::Response& res) {
    json body = json::parse(req.body);
    string sessionCode = body["sessionCode"];
    string name = body["name"];
    string studentID = body["studentID"];
    string deviceID = body["deviceID"];

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

    if (session -> closed) {
        res.status = 400;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "session has ended"}
            }).dump(),
            "application/json");
        return;
    }

    if (session -> markedDevices.count(deviceID)) {
        res.status = 400;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "this device has already marked attendance"}
            }).dump(),
            "application/json");
        return;
    }

    if (session -> markedIDs.count(studentID)) {
        res.status = 400;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "this student ID has already been marked"}
            }).dump(),
            "application/json");
        return;
    }

    if (session -> sessionType == "face_to_face") {
        double lat = body["lat"];
        double lng = body["lng"];
        double distance = calculateDistance(session -> centerLat, session -> centerLng, lat, lng);
        const double allowedRadius = 1000.0;

        if (distance > allowedRadius) {
            res.status = 400;
            res.set_content(
                json({
                    {"status", "error"},
                    {"reason", "You are too far from the class location. Get close and try again"}
                }).dump(),
                "application/json");
            return;
        }
    }

    AttendanceRecord record;
    record.name = name;
    record.studentID = studentID;
    record.deviceID = deviceID;
    record.timeMarked = currentTimeString();
    record.manual = false;

    session -> marked.push_back(record);
    session -> markedIDs.insert(studentID);
    session -> markedDevices.insert(deviceID);

    res.set_content(
        json({
            {"status", "success"}
        }).dump(),
        "application/json");
}


void handleManualMark(const httplib::Request& req, httplib::Response& res) {
    json body = json::parse(req.body);
    string token = body["token"];
    string sessionCode = body["sessionCode"];
    string name = body["name"];
    string studentID = body["studentID"];
    
    string lecturerEmail;

    if (!isTokenValid(token, lecturerEmail)) {
        res.status = 401;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "invalid token"}
            }).dump(),
            "application/json");
        return;
    }

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

    if (session -> closed) {
        res.status = 400;
        res.set_content(
            json({
                {"status", "error"},
                {"reason", "session has ended"}
            }).dump(),
            "application/json");
        return;
    }

    if (session -> markedIDs.count(studentID)) {
        res.status = 400;
        res.set_content(
            json({
                {"status",  "error"},
                {"reason", "this student ID has already been marked"}
            }).dump(),
            "application/json");
        return;
    }

    AttendanceRecord record;
    record.name = name;
    record.studentID = studentID;
    record.deviceID = "manual";
    record.timeMarked = currentTimeString();
    record.manual = true;

    session->marked.push_back(record);
    session->markedIDs.insert(studentID);

    res.set_content(
        json({
            {"status", "success"}
        }).dump(),
        "application/json");
}