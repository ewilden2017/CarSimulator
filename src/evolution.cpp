#include "evolution.h"

#include "detectLine.h"
#include "car.h"
#include "simulation.h"
#include "wall.h"

#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <stdio.h>
#include <fstream>
#include <vector>
#include <iostream>

NEAT::Population* carTest(int gens, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    char curword[20];
    int id;
    NEAT::Genome* startGenome;
    NEAT::Population *pop;
    int evals[NEAT::num_runs];  //Hold records for each run
    int genes[NEAT::num_runs];
    int nodes[NEAT::num_runs];
    
    std::ifstream inFile;
	inFile.open("startgenes");
    printf("Reading initial genes...\n");
    inFile >> curword;
    inFile >> id;
    startGenome = new NEAT::Genome(1, inFile); //TODO FREE
    inFile.close();
    printf("Done.\n");
    
    for (int run = 0; run < NEAT::num_runs; run++) {
        printf("Spawning Population\n");
        pop = new NEAT::Population(startGenome,NEAT::pop_size);
        
        printf("Verifying Spawned Pop\n");
        pop->verify();
        
        for (int gen = 1; gen < gens; gen++) {
            printf("Generation %i\n", gen);
            
            char filename[50];
            sprintf(filename, "Generations/gen_%i", gen);
            
            int highest = carEpoch(pop, gen, filename, walls, path, distances, window);
            
        }
    }
}

int carEpoch(NEAT::Population* pop, int generation, char *filename, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    std::vector<NEAT::Organism*>::iterator currentOrg;
    std::vector<NEAT::Species*>::iterator currentSpecies;
    
    std::vector<Car*> cars;
    
    int highest = -1;
    for(currentOrg = (pop->organisms).begin(); currentOrg != (pop->organisms).end(); ++currentOrg) {
        cars.push_back(new Car(glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0), *currentOrg));
        printf("Hello!\n");
    }
    carSimulation(cars, walls, path, distances, window);
    for(currentSpecies = (pop->species).begin(); currentSpecies != (pop->species).end(); ++currentSpecies) {

        (*currentSpecies)->compute_average_fitness();
        (*currentSpecies)->compute_max_fitness();
    }
    
    if  ((generation % (NEAT::print_every) == 0)) {
        pop->print_to_file_by_species(filename);
    }
	double high = pop->highest_fitness;
    
    pop->epoch(generation);
	return high;
}
    
