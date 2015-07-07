#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //remember to use radians
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "wall.h"

Wall::Wall(glm::vec3 pointA, glm::vec3 pointB) {
    float Ax = pointA.x;
    float Ay = pointA.y;
    float Bx = pointB.x;
    float By = pointB.y;
    
    float midX = (Ax + Bx) / 2;
    float midY = (Ay + By) / 2;
    
    float rotation = atan2(By - Ay, Bx - Ax);
    
    //Yes, this is the intended order.
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(midX,midY,0.0f));
    modelMatrix = glm::rotate(translationMatrix, rotation, glm::vec3(0.0f,0.0f,1.0f));
    angle = rotation;
}

Wall::~Wall() {
    //nothing yet
}

glm::mat4 getMatrix() {
    return modelMatrix;
}

float getAngle {
    return angle;
}