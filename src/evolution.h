#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "population.h"
#include "organism.h"
#include "wall.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int OUTPUTS = 2;

NEAT::Population* carTest(int gens, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window);
int carEpoch(NEAT::Population* pop, int generation, char *filename, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window);
void carEvaluate(NEAT::Organism *org);

#endif
