#ifndef COLLISION_H
#define COLLISION_H
//basic circular collision
bool collisionCircleCircle(glm::vec3 centerA, float radiusA, glm::vec3 centerB, float raduisB);

//Uses corners in clockwise order, detects collision
bool collisionRectSAT(glm::vec3 URA, glm::vec3 LRA, glm::vec3 LLA, glm::vec3 ULA, glm::vec3 URB, glm::vec3 LRB, glm::vec3 LLB, glm::vec3 ULB);

//Handles checks between Cars and Walls
bool collisionCarWall(Car car, Wall wall);
#endif