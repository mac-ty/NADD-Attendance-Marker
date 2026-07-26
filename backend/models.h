#pragma once
#include <string>
#include <vector>
#include <set>
#include <ctime>
using namespace std;

struct Lecturer {
    int id;
    string name;
    string email;
    string passwordHash;
    string repEmail;
    string courseCode;
};

struct AttendanceRecord {
    string name;
    string studentID;
    string deviceID;
    string timeMarked;
    bool manual;
};

struct Session {
    string sessionCode;
    int lecturerID;
    string courseCode;
    string csvFilename;
    string sessionType;
    double centerLat = 0, centerLng = 0;
    time_t startTime;
    int durationSeconds = 600;
    bool closed = false;
    bool emailSent = false;
    string lecturerEmail;
    string repEmail;
    bool sendToLecturer = false;

    vector<AttendanceRecord> marked;
    set<string> markedIDs;
    set<string> markedDevices;
};