#pragma once
#include <string>
#include <vector>
#include <set>
#include <ctime>

struct Lecturer {
    int id;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string repEmail;
};

struct AttendanceRecord {
    std::string name;
    std::string studentID;
    std::string deviceID;
    std::string timeMarked;
    bool manual;
};

struct Session {
    std::string sessionCode;
    int lecturerID;
    std::string sessionType;
    double centerLat = 0, centerLng = 0;
    time_t startTime;
    int durationSeconds = 600;
    bool closed = false;

    std::vector<AttendanceRecord> marked;
    std::set<std::string> markedIDs;
    std::set<std::string> markedDevices;
};