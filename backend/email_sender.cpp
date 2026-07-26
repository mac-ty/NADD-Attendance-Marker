#include <curl/curl.h>
#include "email_sender.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
using namespace std;

static const string sessionsDirectory = "./database/sessions/";

static string buildHtmlTable(const string& csvFilename) {
    ifstream file(sessionsDirectory + csvFilename);
    if (!file.is_open()) {
        return "<p>Could not read attendance records.</p>";
    }

    ostringstream html;
    html << "<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\">";
    html << "<tr><th>NAME</th><th>STUDENT ID</th><th>DEVICE ID</th><th>TIME MARKED</th><th>MANUAL</th></tr>";

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

        html << "<tr><td>" << name << "</td><td>" << studentID << "</td><td>" 
             << deviceID << "</td><td>" << timeMarked << "</td><td>" << manual << "</td></tr>";
        }
        html << "</table>";
        file.close();
        return html.str();
}

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    return size * nmemb;
}

static void sendViaMailgun(const string& toEmail, const string& subject, const string& htmlBody) {
    const char* apiKeyEnv = getenv("MAILGUN_API_KEY");
    const char* domainEnv = getenv("MAILGUN_DOMAIN");
    const char* fromEnv = getenv("MAILGUN_FROM");

    cout << "sendViaMailgun called for: " << toEmail << endl;

    if (!apiKeyEnv || !domainEnv || !fromEnv) {
        cerr << "Missing Mailgun environment variables - email not sent." << endl;
        cerr << "apiKeyEnv present: " << (apiKeyEnv != nullptr) << endl;
        cerr << "domainEnv present: " << (domainEnv != nullptr) << endl;
        cerr << "fromEnv present: " << (fromEnv != nullptr) << endl;
        return;
    }

    string apiKey = apiKeyEnv;
    string domain = domainEnv;
    string from = fromEnv;
    string url = "https://api.mailgun.net/v3/" + domain + "/messages";

    cout << "Using domain: " << domain << endl;
    cout << "Using from: " << from << endl;
    cout << "Sending to URL: " << url << endl;

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize curl." << endl;
        return;
    }

    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part;

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "from");
    curl_mime_data(part, from.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "to");
    curl_mime_data(part, toEmail.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "subject");
    curl_mime_data(part, subject.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "html");
    curl_mime_data(part, htmlBody.c_str(), CURL_ZERO_TERMINATED);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, "api");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, apiKey.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // NEW - dumps full request/response detail

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    cout << "Curl result: " << curl_easy_strerror(res) << " | HTTP status: " << httpCode << endl;

    if (res != CURLE_OK) {
        cerr << "Mailgun send failed: " << curl_easy_strerror(res) << endl;
    }

    curl_mime_free(mime);
    curl_easy_cleanup(curl);
}

void sendAttendanceReport(const Session& session) {
    string tableHtml = buildHtmlTable(session.csvFilename);
    string subject = "Attendance Report - " + session.courseCode + " - " + session.sessionCode;
    string body = "<h2>Attendance Report</h2><p>Course: " + session.courseCode + "</p>" + tableHtml;

    if (!session.repEmail.empty()) {
        sendViaMailgun(session.repEmail, subject, body);
    }
    if (session.sendToLecturer && !session.lecturerEmail.empty()) {
        sendViaMailgun(session.lecturerEmail, subject, body);
    }
}