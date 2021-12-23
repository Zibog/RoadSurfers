#pragma once
#include <vector>
#include <string>
#include "Polyhedron.h"

std::vector<std::string> parse(std::string line, std::string delimiter);

void parseObjFile(Polyhedron& my_polyhedron, std::string path);