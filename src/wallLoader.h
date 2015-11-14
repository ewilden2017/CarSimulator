#ifndef WALL_LOADER_H
#define WALL_LOADER_H

#include <vector>
#include "wall.h"
#include <glm/glm.hpp>

const double WALL_DISTANCE = 5.0;

std::vector<glm::vec3> loadWalls(const char* filename, std::vector<Wall> *walls, std::vector<double> *distances);

#endif