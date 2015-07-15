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
    
    center = glm::vec3(midX,midY,0.0f);
    
    float rotation = atan2(By - Ay, Bx - Ax);
    
    float distance = sqrt(pow(Bx - Ax, 2) + pow(By - Ay, 2));
    
    length = distance;
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length/2,1.0f,1.0f));
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), center);
    glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f,0.0f,1.0f));
    modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    angle = rotation;
    
    printf("PointA: %s\nPointB: %s\n midX: %f midY: %f rotation: %f\n",glm::to_string(pointA).c_str(),glm::to_string(pointB).c_str(),midX,midY,rotation);
}

Wall::~Wall() {
    //nothing yet
}

float Wall::getLength() {
    return length;
}

glm::mat4 Wall::getMatrix() {
    return modelMatrix;
}

glm::vec3 Wall::getCenter() {
    return center;
}

float Wall::getAngle() {
    return angle;
}