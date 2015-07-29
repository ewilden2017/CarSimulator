#ifndef COLLISION_H
#define COLLISION_H
//basic circular collision
bool collisionCircleCircle(glm::vec3 centerA, float radiusA, glm::vec3 centerB, float raduisB);

//Uses corners in clockwise order, detects collision
bool collisionRectSAT(glm::vec4 URA, glm::vec4 LRA, glm::vec4 LLA, glm::vec4 ULA,
                      glm::vec4 URB, glm::vec4 LRB, glm::vec4 LLB, glm::vec4 ULB);

//Handles checks between Cars and Walls
bool collisionCarWall(Car car, Wall wall);
#endif