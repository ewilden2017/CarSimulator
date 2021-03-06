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
 
    std::ifstream populationFile;
    populationFile.open("population");
    if (populationFile.good()) {
        printf("Using population file.\n");
        startGenome = NULL;
        populationFile.close();
    } else {
        std::ifstream inFile;
        inFile.open("startgenes");
        printf("Reading initial genes...\n");
        /* inFile >> curword; */
        /* inFile >> id; */
        startGenome = new NEAT::Genome(1, inFile); //TODO FREE
        inFile.close();
        printf("Done.\n");
    }
    
    for (int run = 0; run < NEAT::num_runs; run++) {
        if (glfwWindowShouldClose(window) != 0) {
            break;
        }
        printf("Spawning Population\n");
        if (startGenome != NULL) {
            pop = new NEAT::Population(startGenome,NEAT::pop_size);
        } else {
            pop = new NEAT::Population("population");
        }
        
        printf("Verifying Spawned Pop\n");
        pop->verify();
        
        for (int gen = 1; gen <= gens; gen++) {
            if (glfwWindowShouldClose(window) != 0) {
                break;
            }
            printf("Generation %i\n", gen);
            
            char filename[50];
            sprintf(filename, "Generations/run_%i_gen_%i", run+1, gen);
            
            int highest = carEpoch(pop, gen, filename, walls, path, distances, window);
        }
    }
}

int carEpoch(NEAT::Population* pop, int generation, char *filename, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    std::vector<NEAT::Organism*>::iterator currentOrg;
    std::vector<NEAT::Species*>::iterator currentSpecies;
    
    std::vector<Car*> cars;
    
    glm::vec3 rotation;
    if (path->front() != path->at(1)) {
        rotation = glm::normalize(path->at(1) - path->front());
    } else {
        rotation = glm::vec3(0.0,0.0,0.0);
    }
 

    int highest = -1;
    for(currentOrg = (pop->organisms).begin(); currentOrg != (pop->organisms).end(); ++currentOrg) {
        cars.push_back(new Car(path->at(0),rotation, *currentOrg));
    }
    carSimulation(cars, walls, path, distances, window);
    
    if(glfwWindowShouldClose(window) != 0) {
        return -1;
    }

    for(currentSpecies = (pop->species).begin(); currentSpecies != (pop->species).end(); ++currentSpecies) {

        (*currentSpecies)->compute_average_fitness();
        (*currentSpecies)->compute_max_fitness();
    }
    if  ((generation % (NEAT::print_every) == 0)) {
        printf("Printing...\n");
        pop->print_to_file_by_species(filename);
    }
	double high = pop->highest_fitness;
    
    pop->epoch(generation);
	return high;
}
    
