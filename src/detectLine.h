#ifndef DETECTLINE_H
#define DETECTLINE_H

#include <glm/glm.hpp>

const double PI = 3.14159265354;

const double LINE_START = -PI / 4;
const double LINE_END = PI / 4;
const int LINE_COUNT = 9;

class DetectLine {
public:
    
    DetectLine(glm::vec3 newCenter, double newAngle, double newOffset, double newLength);
    
    glm::vec3 getCenter();
    void setCenter(glm::vec3 newCenter);
    
    glm::mat4 getMatrix();
    
    double getAngle();
    double getOffset();
    
    void setAngle(double newAngle);
    
    glm::vec3 getEnd();
    
    double getDistance();
    void setDistance(double newDistance);
    
private:
    glm::vec3 center;
    
    glm::mat4 translation;
    glm::mat4 rotation;
    glm::mat4 scale;
    
    double angle;
    double offset;
    double length;
    
    double distance;
};
    
#endif
