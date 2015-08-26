#ifndef WALL_H
#define WALL_H

class Wall {
public:
    Wall(glm::vec3 pointA, glm::vec3 pointB);
    ~Wall();
    
    float getLength();
    
    glm::vec3* getVerticies();
    glm::mat4 getMatrix();
    glm::vec3 getCenter();
    float getAngle();

private:
    glm::vec3 pointA;
    glm::vec3 pointB;
    float angle;
    
    glm::vec3 center;
    float length;
    
    glm::vec3 Verticies[4];
    
    glm::mat4 modelMatrix;
};

#endif