#include "detectLine.h"

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "common.h"

#include "CarSimConfig.h"

const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);

DetectLine::DetectLine(glm::vec3 newCenter, double newAngle, double newOffset, double newLength) {
    center = newCenter;
    angle = newAngle;
    offset = newOffset;
    length = newLength;
    
    distance = -1;
    
    scale = glm::scale(glm::mat4(1.0), glm::vec3(0.0,newLength,0.0));
    translation = glm::translate(glm::mat4(1.0), newCenter);
    rotation = glm::rotate(glm::mat4(1.0), (float)newAngle + (float)offset, UP);
    
    printf("Center:%s Angle: %f Offset: %f Length: %f\n", glm::to_string(center).c_str(), angle, offset, length);
}

glm::vec3 DetectLine::getCenter() {
    return center;
}

glm::mat4 DetectLine::getMatrix() {
    return translation * rotation * scale;
}

double DetectLine::getAngle() {
    return angle;
}

double DetectLine::getOffset() {
    return offset;
}

void DetectLine::setCenter(glm::vec3 newCenter) {
    //glm::vec3 lengthVector = glm::rotate(glm::vec3(0.0,length,0.0), (float)angle + (float)offset, UP);
    translation = glm::translate(glm::mat4(1.0), newCenter);
    center = newCenter;
}

void DetectLine::setAngle(double newAngle) {
    rotation = glm::rotate(glm::mat4(1.0), (float)newAngle + (float)offset, UP);
    
    angle = newAngle;
}

glm::vec3 DetectLine::getEnd() {
    return glm::vec3(this->getMatrix() * glm::vec4(0.0,1.0,0.0,1.0));
}

double DetectLine::getDistance() {
    return distance;
}

void DetectLine::setDistance(double newDistance) {
    distance = newDistance;
}