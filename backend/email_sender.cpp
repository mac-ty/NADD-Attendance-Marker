#include <curl/curl.h>
#include "email_sender.h"
#include "json.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
using namespace std;
using json = nlohmann::json;

static const string sessionsDirectory = "./database/sessions/";

static string buildHtmlTable(const string& csvFilename) {
    ifstream file(sessionsDirectory + csvFilename);
    if (!file.is_open()) {
        return "<p>Could not read attendance records.</p>";
    }

    ostringstream html;
    html << "<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\" style=\"width:100%; max-width:600px;\">";
    html << "<tr><th>NAME</th><th>INDEX_NUMBER</th></tr>";

    string line;
    bool isHeaderRow = true;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (isHeaderRow) { isHeaderRow = false; continue; }

        stringstream ss(line);
        string name, studentID, deviceID, timeMarked, manual;
        getline(ss, name, ',');
        getline(ss, studentID, ',');
        getline(ss, deviceID, ',');
        getline(ss, timeMarked, ',');
        getline(ss, manual, ',');

        html << "<tr><td>" << name << "</td><td>" << studentID << "</td></tr>";
    }

    html << "</table>";
    file.close();
    return html.str();
}

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    return size * nmemb;
}

static void sendViaBrevo(const string& toEmail, const string& subject, const string& htmlBody) {
    const char* apiKeyEnv = getenv("BREVO_API_KEY");
    const char* senderEmailEnv = getenv("BREVO_SENDER_EMAIL");

    if (!apiKeyEnv || !senderEmailEnv) {
        cerr << "Missing Brevo environment variables - email not sent." << endl;
        return;
    }

    string apiKey = apiKeyEnv;
    string senderEmail = senderEmailEnv;

    json requestBody = {
        {"sender", {{"email", senderEmail}}},
        {"to", json::array({ {{"email", toEmail}} })},
        {"subject", subject},
        {"htmlContent", htmlBody}
    };
    string bodyStr = requestBody.dump();

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize curl." << endl;
        return;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.brevo.com/v3/smtp/email");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    cout << "Brevo send to " << toEmail << " - result: " << curl_easy_strerror(res)
         << " | HTTP status: " << httpCode << endl;

    if (res != CURLE_OK) {
        cerr << "Brevo send failed: " << curl_easy_strerror(res) << endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void sendAttendanceReport(const Session& session) {
    string tableHtml = buildHtmlTable(session.csvFilename);
    string subject = "Attendance Report - " + session.courseCode + " - " + session.sessionCode;
    string body = "<h2>Attendance Report</h2><p>Course: " + session.courseCode + "</p>" + tableHtml;

    if (!session.repEmail.empty()) {
        sendViaBrevo(session.repEmail, subject, body);
    }

    if (session.sendToLecturer && !session.lecturerEmail.empty()) {
        sendViaBrevo(session.lecturerEmail, subject, body);
    }
}