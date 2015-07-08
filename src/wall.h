#ifndef WALL_H
#define WALL_H

const GLfloat wallData[] = { 
    -1.0f, -0.2f, 0.0f,
    1.0f, -0.2f, 0.0f,
    -1.0f,  0.2f, 0.0f,
    1.0f, 0.2f, 0.0f,
    -1.0f, 0.2f, 0.0f,
    1.0f, -0.2f, 0.0f,
};

class Wall {
public:
    Wall(glm::vec3 pointA, glm::vec3 pointB);
    ~Wall();
    
    float getLength();
    
    glm::vec3* getVerticies();
    glm::mat4 getMatrix();
    float getAngle();

private:
    glm::vec3 pointA;
    glm::vec3 pointB;
    float angle;
    
    glm::vec3 Verticies[4];
    
    glm::mat4 modelMatrix;
};

#endif