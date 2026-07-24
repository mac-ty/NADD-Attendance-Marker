#pragma once
#include <string>
#include "httplib.h"

void handleStartSession(const httplib::Request& req, const httplib::Response& res);
void handleSessionStatus(const httplib::Request& req, const httplib::Response& res);
void handleCloseSession(const httplib::Request& req, const httplib::Response& res);