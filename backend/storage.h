#pragma once
#include <vector>
#include "models.h"
using namespace std;

vector<Lecturer> loadLecturers();
void appendLecturer(const Lecturer &lecturer);

void createSessionFile(const Session &session);
void appendAttendanceRecord(const Session &session, const AttendanceRecord &record);