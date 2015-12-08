#include "evolution.h"

#include "detectLine.h"

#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"

#include <stdio.h>
#include <ifstream>
#include <vector>

NEAT::Population* carTest(int gens) {
    char curword[20];
    int id;
    NEAT::Genome* startGenome;
    Population *pop;
    
    int evals[NEAT::num_runs];  //Hold records for each run
    int genes[NEAT::num_runs];
    int nodes[NEAT::num_runs];
    
    ifstream inFile = ifstream("startgenes")
    printf("Reading initial genes...");
    inFile>>curword;
    inFile>>id;
    startGenome = new Genome(id, inFile); //TODO FREE
    inFile.close();
    printf("Done.\n");
    
    for (int run = 0; run < NEAT::num_runs; run++) {
        printf("Spawning Population\n");
        pop = new Population(startGenome,NEAT::pop_size);
        
        cout<<"Verifying Spawned Pop"<<endl;
        pop->verify();
        
        for (int gen = 1; gen < gens; gen++) {
            printf("Generation %i\n", gen);
            
            char filename[50];
            sprintf(filename, "Generations/gen_%i", gen);
            
            int winnerNum;
            int winnerGenes;
            int winnerNodes;
            
            int highest = carEpoch(pop, gen, filename, winnerNum, winnerGenes, winnerNodes);
            
        }
    }
}

bool carEpoch(NEAT::Population* pop, int generation, char *filename, int &winnernum, int &winnergenes,int &winnernodes) {
    std::vector<NEAT::Organism*>::iterator currentOrg;
    std::vector<NEAT::Species*>::iterator currentSpecies;
    
    int highest = -1;
    for(currentOrg = (pop->organisms).begin(); currentOrg != (pop->organisms).end(); ++currentOrg) {
        carEvaluate(*currentOrg);
    }
    
    for(currentSpecies = (pop->species).begin(); currentSpecies != (pop->species).end(); ++currentSpecies) {

        (*currentSpecies)->compute_average_fitness();
        (*currentSpecies)->compute_max_fitness();
    }
    
    if  ((generation % (NEAT::print_every) == 0)) {
        pop->print_to_file_by_species(filename);
    }
    
    pop->epoch(generation);
}

void carEvaluate(NEAT::Organism *org) {
    Network *net;
    out[OUTPUTS];
    