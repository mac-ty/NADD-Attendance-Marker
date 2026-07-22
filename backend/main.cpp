#include "httplib.h"
#include "auth.h"
#include <iostream>
using namespace std;

int main() {
    httplib::Server svr;

    svr.Post("/api/lecturer/signup", handleSignup);
    svr.Post("/api/lecturer/login", handleLogin);

    cout << "Server running at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}