// Written by: Theodore Macaulay Esene 2526403511

#pragma once
#include "httplib.h"

void handleMarkAttendance(const httplib::Request& req, httplib::Response& res);
void handleManualMark(const httplib::Request& req, httplib::Response& res);