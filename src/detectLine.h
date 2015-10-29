#ifndef DETECTLINE_H
#define DETECTLINE_H

#include <glm/glm.hpp>

const double PI = 3.14159265354;

const double LINE_START = -PI / 4;
const double LINE_END = PI / 4;
<<<<<<< HEAD
const int LINE_COUNT = 1;
=======
const int LINE_COUNT = 9;
>>>>>>> b78d6fc1e7f1489da45cc750380a380b41abd915

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
