#include "httplib.h"
#include "auth.h"
#include "session_manager.h"
#include "attendance.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
    httplib::Server svr;

    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });
    
    svr.Post("/api/lecturer/signup", handleSignup);
    svr.Post("/api/lecturer/login", handleLogin);
    svr.Post("/api/session/start", handleStartSession);
    svr.Get(R"(/api/session/(\w+)/status)", handleSessionStatus);
    svr.Post(R"(/api/session/(\w+)/close)", handleCloseSession);
    svr.Post("/api/attendance/mark", handleMarkAttendance);
    svr.Post("/api/attendance/manual", handleManualMark);
    // svr.Get("/api/debug/lecturers", handleDebugLecturers);

    const char* portEnv = getenv("PORT");
    int port = portEnv ? atoi(portEnv) : 8080;

    cout << "Server running on port " << port << endl;
    svr.listen("0.0.0.0", port);

    return 0;
}