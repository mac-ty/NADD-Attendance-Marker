#include "storage.h"
#include <fstream>
#include <sstream>
using namespace std;


static const string lecturerFile = "../database/lecturers.csv";


vector<Lecturer> loadLecturers() {
    vector<Lecturer> lecturers;
    ifstream file(lecturerFile);

    if (!file.is_open()) {
        return lecturers;
    }

    string lecturerRecord;
    while (getline(file, lecturerRecord)) {
        if (lecturerRecord.empty()) continue;
        if (lecturerRecord == "id, name, email, passwordHash, repEmail") continue;

        stringstream ss(lecturerRecord);
        string id, name, email, passwordHash, repEmail;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, email, ',');
        getline(ss, passwordHash, ',');
        getline(ss, repEmail, ',');

        Lecturer lecturer;
        lecturer.id = stoi(id);
        lecturer.name = name;
        lecturer.email = email;
        lecturer.passwordHash = passwordHash;
        lecturer.repEmail = repEmail;

        lecturers.push_back(lecturer);
    }

    file.close();
    return lecturers;
}


void appendLecturer(const Lecturer& lecturer) {
    ofstream file(lecturerFile, ios::app);
    file << lecturer.id << "," << lecturer.name << "," << lecturer.email << "," << lecturer.passwordHash << "," << lecturer.repEmail << "\n";
    file.close();
}
