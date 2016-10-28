#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

#include "car.h"
#include "wall.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void carSimulation(std::vector<Car*> cars, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window);

#endif
