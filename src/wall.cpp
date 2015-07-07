#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "wall.h"
const glm::vec3 UP = glm::vec3(0.0f,0.0f,-1.0f);

Wall::Wall(glm::vec3 pointA, glm::vec3 pointB) {
    modelMatrix = glm::mat4(1.0f);
    
    float Ax = pointA.x;
    float Ay = pointA.y;
    float Bx = pointB.x;
    float By = pointB.y;
    
    float midX = (Ax + Bx) / 2;
    float midY = (Ay + By) / 2;
    
    float rotation = atan2(By - Ay, Bx - Ax) * 180 / M_PI;
    
    //Yes, this is the intended order.
    modelMatrix = glm::translate(modelMatrix, glm::vec3(midX,midY,0.0f));
    modelMatrix = glm::rotate(modelMatrix, rotation, UP);
    angle = rotation;
}

Wall::~Wall() {
}