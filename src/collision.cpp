#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <vector>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "wall.h"
#include "car.h"

#include "collision.h"


bool collisionCircleCircle(glm::vec3 centerA, float radiusA, glm::vec3 centerB, float radiusB) {
    float distanceX = centerB.x - centerA.x;
    float distanceY = centerB.y - centerB.y;
    
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    
    return distanceSquared < (radiusA + radiusB) * (radiusA + radiusB);
}

bool collisionRectSAT(glm::vec3 URA, glm::vec3 LRA, glm::vec3 LLA, glm::vec3 ULA, glm::vec3 URB, glm::vec3 LRB, glm::vec3 LLB, glm::vec3 ULB) {
    glm::vec3 axis1;
    glm::vec3 axis2;
    glm::vec3 axis3;
    glm::vec3 axis4;
    
    axis1.x = URA.x - ULA.x;
    axis1.y = URA.y - ULA.y;
    axis2.x = URA.x - LRA.x;
    axis2.y = URA.y - LRA.y;
    axis3.x = ULB.x - LLB.x;
    axis3.y = ULB.y - LLB.y;
    axis4.x = ULB.x - URB.x;
    axis4.y = ULB.y - URB.y;
}