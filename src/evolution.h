#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "population.h"
#include "organism.h"

const int OUTPUTS = 2;

NEAT::Population* carTest(int gens);
int carEpoch(NEAT::Population* pop, int generation, char *filename, int &winnernum, int &winnergenes,int &winnernodes);
void carEvaluate(NEAT::Organism *org);

#endif