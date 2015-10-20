#include "config.h"
#include "wall.h"

#include <stdio.h>
#include <math.h>

#define GLM_FORCE_RADIANS //remember to use radians
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Wall::Wall(glm::vec3 pointA, glm::vec3 pointB) {
    double Ax = pointA.x;
    double Ay = pointA.y;
    double Bx = pointB.x;
    double By = pointB.y;
    
    double midX = (Ax + Bx) / 2;
    double midY = (Ay + By) / 2;
    
    center = glm::vec3(midX,midY,0.0);
    
    float rotation = atan2(By - Ay, Bx - Ax);
    
    double distance = sqrt(pow(Bx - Ax, 2) + pow(By - Ay, 2));
    
    length = distance;
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(length/2,1.0,1.0));
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0), center);
    glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), rotation, glm::vec3(0.0,0.0,1.0));
    modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    angle = rotation;
    
    printf("PointA: %s\nPointB: %s\n midX: %f midY: %f rotation: %f\n",glm::to_string(pointA).c_str(),glm::to_string(pointB).c_str(),midX,midY,rotation);
}

double Wall::getLength() {
    return length;
}

glm::mat4 Wall::getMatrix() {
    return modelMatrix;
}

glm::vec3 Wall::getCenter() {
    return center;
}

double Wall::getAngle() {
    return angle;
}