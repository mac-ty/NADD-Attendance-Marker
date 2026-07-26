#pragma once
#include <string>   
#include "httplib.h"
using namespace std;

void handleSignup(const httplib::Request& req, httplib::Response& res);
void handleLogin(const httplib::Request& req, httplib::Response& res);
void handleDebugLecturers(const httplib::Request& req, httplib::Response& res);
bool isTokenValid(const string& token, string& outEmail);