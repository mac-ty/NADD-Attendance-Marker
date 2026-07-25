#pragma once
#include "httplib.h"

void handleMarkAttendance(const httplib::Request& req, httplib::Response& res);
void handleManualMark(const httplib::Request& req, httplib::Response& res);