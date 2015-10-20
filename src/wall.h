#ifndef WALL_H
#define WALL_H

#include <glm/glm.hpp>

class Wall {
public:
    Wall(glm::vec3 pointA, glm::vec3 pointB);
    
    double getLength();
    
    glm::vec3* getVerticies();
    glm::mat4 getMatrix();
    glm::vec3 getCenter();
    double getAngle();

private:
    glm::vec3 pointA;
    glm::vec3 pointB;
    double angle;
    
    glm::vec3 center;
    double length;
    
    glm::vec3 Verticies[4];
    
    glm::mat4 modelMatrix;
};

#endif