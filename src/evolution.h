#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "population.h"
#include "organism.h"
#include "wall.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int OUTPUTS = 2;

NEAT::Population* carTest(int gens, std::vector<Wall>* walls, GLFWwindow* window);
int carEpoch(NEAT::Population* pop, int generation, char *filename, int &winnernum, int &winnergenes,int &winnernodes, std::vector<Wall>* walls, GLFWwindow* window);
void carEvaluate(NEAT::Organism *org);

#endif
