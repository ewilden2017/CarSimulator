#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "wall.h"
#include "car.h"

#include "collision.h"

const static float carRadius = 1.118 * CAR_SCALE_FACTOR; //for now, is constant

bool collisionCircleCircle(glm::vec3 centerA, float radiusA, glm::vec3 centerB, float radiusB) {
    float distanceX = centerB.x - centerA.x;
    float distanceY = centerB.y - centerB.y;
    
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    
    return distanceSquared < ((radiusA + radiusB) * (radiusA + radiusB));
}

bool collisionRectSAT(glm::vec4 URA4, glm::vec4 LRA4, glm::vec4 LLA4, glm::vec4 ULA4,
                      glm::vec4 URB4, glm::vec4 LRB4, glm::vec4 LLB4, glm::vec4 ULB4) {
    
    glm::vec3 URA(URA4);
    glm::vec3 LRA(LRA4);
    glm::vec3 LLA(LLA4);
    glm::vec3 ULA(ULA4);
    
    glm::vec3 URB(URB4);
    glm::vec3 LRB(LRB4);
    glm::vec3 LLB(LLB4);
    glm::vec3 ULB(ULB4);
    
    glm::vec3 axis[4];
    
    axis[0].x = URA.x - ULA.x;
    axis[0].y = URA.y - ULA.y;
    axis[1].x = URA.x - LRA.x;
    axis[1].y = URA.y - LRA.y;
    axis[2].x = ULB.x - LLB.x;
    axis[2].y = ULB.y - LLB.y;
    axis[3].x = ULB.x - URB.x;
    axis[3].y = ULB.y - URB.y;
    
    glm::vec3 vertA[] = {URA, LRA, LLA, ULA};
    glm::vec3 vertB[] = {URB, LRB, LLB, ULB};
    
    glm::vec3 projA[4];
    glm::vec3 projB[4];
    
    std::vector<float> positionA(4);
    std::vector<float> positionB(4);
    for (int a = 0; a < 4; a++) {
        for (int i = 0; i < 4; i++) {
            float top = vertA[i].x * axis[a].x + vertA[i].y * axis[a].y;
            float bottom = axis[a].x * axis[a].x + axis[a].y * axis[a].y;
        
            float common = top / bottom;
        
            projA[i].x = common * axis[a].x;
            projA[i].y = common * axis[a].y;
        
            positionA.at(i) = glm::dot(projA[i], axis[a]);
        }
    
        for (int i = 0; i < 4; i++) {
            float top = vertB[i].x * axis[a].x + vertB[i].y * axis[a].y;
            float bottom = axis[a].x * axis[a].x + axis[a].y * axis[a].y;
        
            float common = top / bottom;
        
            projB[i].x = common * axis[a].x;
            projB[i].y = common * axis[a].y;
        
            positionB.at(i) = glm::dot(projB[i], axis[a]);
        }
        float minA = *(std::min_element(positionA.begin(), positionA.end()));
        float minB = *(std::min_element(positionB.begin(), positionB.end()));
        float maxA = *(std::max_element(positionA.begin(), positionA.end()));
        float maxB = *(std::max_element(positionB.begin(), positionB.end()));
    
        //no collision on this axis, none at all
        if (((minB > maxA) || (maxB < minA))) {
            return false;
        }
    }
    return true;
}

bool collisionCarWall(Car& car, Wall& wall, glm::mat4 carMatrix) {
    //can't collide if they are too far apart
    if (!collisionCircleCircle(car.getCenter(), carRadius, wall.getCenter(), wall.getLength()/2 + 1)) { //not very wide, and too lazy to do more percise math
        return false;
    }
    
    printf("Checking\n");
    glm::mat4 wallMatrix = wall.getMatrix();
    
    glm::vec4 carUR = carMatrix * glm::vec4(0.5,1.0,0.0,1.0);
    glm::vec4 carLR = carMatrix * glm::vec4(0.5,-1.0,0.0,1.0);
    glm::vec4 carLL = carMatrix * glm::vec4(-0.5,-1.0,0.0,1.0);
    glm::vec4 carUL = carMatrix * glm::vec4(-0.5,1.0,0.0,1.0);
    
    glm::vec4 wallUR = wallMatrix * glm::vec4(1.0,0.2,0.0,1.0);
    glm::vec4 wallLR = wallMatrix * glm::vec4(1.0,-0.2,0.0,1.0);
    glm::vec4 wallLL = wallMatrix * glm::vec4(-1.0,-0.2,0.0,1.0);
    glm::vec4 wallUL = wallMatrix * glm::vec4(-1.0,0.2,0.0,1.0);
    
    return collisionRectSAT(carUR, carLR, carLL, carUL,
       wallUR, wallLR, wallLL, wallUL);

}