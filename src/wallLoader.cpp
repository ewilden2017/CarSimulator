#include "config.h"
#include "wallLoader.h"

#include "wall.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "common.h"

std::vector<glm::vec3> loadWalls(const char* filename, std::vector<Wall> *walls) {
    std::vector<glm::vec3> path;
    std::ifstream wallFile(filename);
    
    if (wallFile.is_open()) {
    
        std::vector<double> numbers;
        
        std::string line;
        
        std::string token;
        std::stringstream ss;
        //get numbers from file
        while(std::getline(wallFile, line)) {
            
            ss << line;
            while (getline(ss, token, ',')) {
                double f;
                f = strtof(token.c_str(), NULL);
                numbers.push_back(f);
            }
            ss.clear();
        }
        //convert numbers to points
        for (int i = 0; i < numbers.size(); i += 3) {
            if (numbers.size() < i+3) {
                printf("Not enough numbers for another path segment.\n");
            } else {
                glm::vec3 point = glm::vec3(numbers.at(i), numbers.at(i+1), numbers.at(i+2));
                path.push_back(point);
            }
        }
    } else {
        printf("Could not open wall file.\n");
        path.push_back(glm::vec3(0.0,0.0,0.0));
        path.push_back(glm::vec3(0.0,99.0,0.0));
    }
    
    if (path.size() < 2) {
        printf("Not enough walls, adding some...\n");
        
        path.push_back(glm::vec3(0.0,0.0,0.0));
        path.push_back(glm::vec3(0.0,99.0,0.0));
    }
    
    //convert points to walls
    
    glm::vec3 firstPoint = path.front();
    glm::vec3 secondPoint = path.at(1);
    
    double firstAngle = PI;
    if ( secondPoint.x != firstPoint.x) {
        firstAngle = atan2((secondPoint.y - firstPoint.y), (secondPoint.x - firstPoint.x));
    }
    
    glm::vec3 lastIntersectUp;
    lastIntersectUp.x = WALL_DISTANCE * cos(firstAngle + PI/2) + firstPoint.x;
    lastIntersectUp.y = WALL_DISTANCE * sin(firstAngle + PI/2) + firstPoint.y;
    
    glm::vec3 lastIntersectDown;
    lastIntersectDown.x = WALL_DISTANCE * cos(firstAngle - PI/2) + firstPoint.x;
    lastIntersectDown.y = WALL_DISTANCE * sin(firstAngle - PI/2) + firstPoint.y;
    
    for (std::vector<glm::vec3>::iterator it = path.begin() + 1; it != path.end() - 1; it++) {
        
        //not working. Hope it is safe enough.
        /*if (!(it - 1) || !(it + 1)) {
            continue;
        }
        */
        
        glm::vec3 point0 = *(it - 1);
        glm::vec3 point1 = *it;
        glm::vec3 point2 = *(it + 1);
        
        printf("X: %f ,Y: %f\n", point1.x, point1.y);
        
        double angle1 = PI;
        if (point1.x != point0.x) {
            angle1 = atan2((point1.y - point0.y), (point1.x - point0.x));
        }
        
        double angle2 = PI;
        if (point2.x != point1.x) {
            angle2 = atan2((point2. y - point1.y), (point2.x - point1.x));
        }
        
        printf("Angles:%f, %f\n", angle1, angle2);
        
        double intersectLength = WALL_DISTANCE;
        if (abs(angle1 - angle2) != PI) {
            intersectLength = WALL_DISTANCE / cos((angle2 - angle1) / 2);
        }
        printf("%f\n", intersectLength);
        
        glm::vec3 intersectUp;
        intersectUp.x = intersectLength * cos(((angle1 + angle2) / 2) + PI/2) + point1.x;
        intersectUp.y = intersectLength * sin(((angle1 + angle2) / 2) + PI/2) + point1.y;
        
        glm::vec3 intersectDown;
        intersectDown.x = intersectLength * cos(((angle1 + angle2) / 2) - PI/2) + point1.x;
        intersectDown.y = intersectLength * sin(((angle1 + angle2) / 2) - PI/2) + point1.y;
        
        walls->push_back(Wall(lastIntersectUp, intersectUp));
        walls->push_back(Wall(lastIntersectDown, intersectDown));
        
        lastIntersectUp = intersectUp;
        lastIntersectDown = intersectDown;
    }
    
    //add final segment
    
    glm::vec3 prevPoint = path.at(path.size() - 2); //2 for 0-indexed
    glm::vec3 finalPoint = path.back();
    
    double finalAngle = PI;
    if ( secondPoint.x != firstPoint.x) {
        finalAngle = atan2((finalPoint.y - prevPoint.y), (finalPoint.x - prevPoint.x));
    }
    
    printf("Angle: %f\n", finalAngle);
    
    glm::vec3 finalIntersectUp;
    finalIntersectUp.x = WALL_DISTANCE * cos(finalAngle + PI/2) + finalPoint.x;
    finalIntersectUp.y = WALL_DISTANCE * sin(finalAngle + PI/2) + finalPoint.y;
    
    glm::vec3 finalIntersectDown;
    finalIntersectDown.x = WALL_DISTANCE * cos(finalAngle - PI/2) + finalPoint.x;
    finalIntersectDown.y = WALL_DISTANCE * sin(finalAngle - PI/2) + finalPoint.y;
    
    walls->push_back(Wall(lastIntersectUp, finalIntersectUp));
    walls->push_back(Wall(lastIntersectDown, finalIntersectDown));
    
    return path;
}