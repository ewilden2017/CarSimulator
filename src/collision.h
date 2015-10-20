#ifndef COLLISION_H
#define COLLISION_H

class Car;
class Wall;

#include <glm/glm.hpp>

//basic circular collision
bool collisionCircleCircle(glm::vec3 centerA, double radiusA, glm::vec3 centerB, double raduisB);

//Uses corners in clockwise order, detects collision
bool collisionRectSAT(glm::vec4& URA, glm::vec4& LRA, glm::vec4& LLA, glm::vec4& ULA,
                      glm::vec4& URB, glm::vec4& LRB, glm::vec4& LLB, glm::vec4& ULB);

//Handles checks between Cars and Walls
bool collisionCarWall(Car& car, Wall& wall, glm::mat4 carMatrix);

//Checks where two lines intersect, and sets out to distance along line A
bool collisionLineLine(const glm::vec3& A1, const glm::vec3& A2, const glm::vec3& B1, const glm::vec3& B2, double* out);
#endif