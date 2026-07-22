#pragma once
#include <vector>
#include "models.h"

std::vector<Lecturer> loadLecturers();
void appendLecturer(const Lecturer& lec);