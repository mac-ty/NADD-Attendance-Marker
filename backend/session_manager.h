#pragma once
#include <string>
#include "httplib.h"
#include "models.h"
using namespace std;

void handleStartSession(const httplib::Request& req, httplib::Response& res);
void handleSessionStatus(const httplib::Request& req, httplib::Response& res);
void handleCloseSession(const httplib::Request& req, httplib::Response& res);   
void handleDebugSessionFile(const httplib::Request& req, httplib::Response& res);

Session* findSession(const string& sessionCode);