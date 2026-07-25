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
    string sessionType;
    double centerLat = 0, centerLng = 0;
    time_t startTime;
    int durationSeconds = 600;
    bool closed = false;

    vector<AttendanceRecord> marked;
    set<string> markedIDs;
    set<string> markedDevices;
};