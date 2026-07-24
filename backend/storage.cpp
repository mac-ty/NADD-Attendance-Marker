#include "storage.h"
#include <fstream>
#include <sstream>
using namespace std;


static const string lecturerFile = "./database/lecturers.csv";


vector<Lecturer> loadLecturers() {
    vector<Lecturer> lecturers;
    ifstream file(lecturerFile);

    if (!file.is_open()) {
        return lecturers;
    }

    string lecturerRecord;
    while (getline(file, lecturerRecord)) {
        if (lecturerRecord.empty()) continue;
        if (lecturerRecord == "id, name, email, repEmail, courseCode, passwordHash") continue;

        stringstream ss(lecturerRecord);
        string id, name, email, passwordHash, repEmail, courseCode;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, email, ',');
        getline(ss, repEmail, ',');
        getline(ss, courseCode, ',');
        getline(ss, passwordHash, ',');

        Lecturer lecturer;
        lecturer.id = stoi(id);
        lecturer.name = name;
        lecturer.email = email;
        lecturer.passwordHash = passwordHash;
        lecturer.repEmail = repEmail;
        lecturer.courseCode = courseCode;

        lecturers.push_back(lecturer);
    }

    file.close();
    return lecturers;
}


void appendLecturer(const Lecturer& lecturer) {
    ofstream file(lecturerFile, ios::app);
    file << lecturer.id << "," << lecturer.name << "," << lecturer.email << "," <<  lecturer.repEmail << "," <<lecturer.courseCode << "," << lecturer.passwordHash << "\n";
    file.close();
}