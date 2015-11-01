#include "config.h"
#include "wallLoader.h"

#include "wall.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

std::vector<Wall> loadWalls(const char* filename) {
    std::ifstream wallFile(filename);
    
    if (wallFile.is_open()) {
    
        std::vector<double> numbers;
        
        std::string line;
        while(std::getline(wallFile, line, ',')) {
            
            double f;
            
            f = strtof(line.c_str(), NULL);
            numbers.push_back(f);
        }
        
        std::vector<Wall> walls;
        for (int i = 0; i < numbers.size(); i += 6) {
            if (numbers.size() < i+6) {
                printf("Not enough numbers for another wall.\n");
            } else {
                glm::vec3 A = glm::vec3(numbers.at(i), numbers.at(i+1), numbers.at(i+2));
                glm::vec3 B = glm::vec3(numbers.at(i+3), numbers.at(i+4), numbers.at(i+5));
                walls.push_back(Wall(A, B));
            }
        }
        return walls;
    } else {
        printf("Could not open wall file.");
        return std::vector<Wall>();
    }
    
}