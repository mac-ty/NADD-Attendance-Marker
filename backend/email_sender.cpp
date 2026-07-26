#include <curl/curl.h>
#include "email_sender.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <ctime>
using namespace std;

static const string sessionsDirectory = "./database/sessions/";

static string buildHtmlTable(const string& csvFilename) {
    ifstream file(sessionsDirectory + csvFilename);
    if (!file.is_open()) {
        return "<p>Could not read attendance records.</p>";
    }

    ostringstream html;
    html << "<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\">";
    html << "<tr><th>name</th><th>studentID</th><th>deviceID</th><th>timeMarked</th><th>manual</th></tr>";

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

struct UploadContext {
    string data;
    size_t pos = 0;
};

static size_t readCallback(char* ptr, size_t size, size_t nmemb, void* userp) {
    UploadContext* ctx = (UploadContext*)userp;
    size_t bufferSize = size * nmemb;
    size_t remaining = ctx->data.size() - ctx->pos;
    size_t toCopy = min(bufferSize, remaining);

    if (toCopy == 0) return 0;

    memcpy(ptr, ctx->data.c_str() + ctx->pos, toCopy);
    ctx->pos += toCopy;
    return toCopy;
}

static void sendViaGmailSmtp(const string& toEmail, const string& subject, const string& htmlBody) {
    const char* gmailAddressEnv = getenv("GMAIL_ADDRESS");
    const char* gmailAppPasswordEnv = getenv("GMAIL_APP_PASSWORD");

    if (!gmailAddressEnv || !gmailAppPasswordEnv) {
        cerr << "Missing Gmail environment variables - email not sent." << endl;
        return;
    }

    string gmailAddress = gmailAddressEnv;
    string gmailAppPassword = gmailAppPasswordEnv;

    ostringstream message;
    message << "To: " << toEmail << "\r\n";
    message << "From: " << gmailAddress << "\r\n";
    message << "Subject: " << subject << "\r\n";
    message << "MIME-Version: 1.0\r\n";
    message << "Content-Type: text/html; charset=UTF-8\r\n";
    message << "\r\n";
    message << htmlBody << "\r\n";

    UploadContext ctx;
    ctx.data = message.str();

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize curl." << endl;
        return;
    }

    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, toEmail.c_str());

    string mailFrom = "<" + gmailAddress + ">";

    curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
    curl_easy_setopt(curl, CURLOPT_USERNAME, gmailAddress.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, gmailAppPassword.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mailFrom.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "Gmail SMTP send failed: " << curl_easy_strerror(res) << endl;
    }
    else {
        cout << "Email sent successfully to " << toEmail << endl;
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
}

void sendAttendanceReport(const Session& session) {
    string tableHtml = buildHtmlTable(session.csvFilename);
    string subject = "Attendance Report - " + session.courseCode + " - " + session.sessionCode;
    string body = "<h2>Attendance Report</h2><p>Course: " + session.courseCode + "</p>" + tableHtml;

    if (!session.repEmail.empty()) {
        sendViaGmailSmtp(session.repEmail, subject, body);
    }

    if (session.sendToLecturer && !session.lecturerEmail.empty()) {
        sendViaGmailSmtp(session.lecturerEmail, subject, body);
    }
}