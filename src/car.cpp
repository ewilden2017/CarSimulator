#include "car.h"

#include <stdio.h>
#include <math.h>
#include <stdexcept>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "neat.h"
#include "organism.h"

#include "collision.h"
#include "wall.h"
#include "detectLine.h"

#include "CarSimConfig.h"

const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);

std::vector<Car*> Car::carList;
int Car::carCount = 0;
int Car::selected = -1;

glm::mat4 Car::Camera = glm::mat4(1.0);
double Car::speedMulti = 1.0;

Car::Car(glm::vec3 newCenter, glm::vec3 forward) {
    if (Car::carCount >= MAX_CARS) {
        printf("Too many cars!\n");
        throw std::out_of_range("Max car limit exceeded.");
    }
    
    index = Car::carCount++;
    /* Car::carList.push_back(this); */
    
    /* printf("Making car %i\n", index); */
    
	pointDist = 0;
	stuckTimer = 0.0;
    nextPoint = 1;

    forwardVector = forward;    
    speed = 0.0;
    acceleration = 0.0;
        
    modelMatrix = glm::mat4(1.0);
    
    center = glm::vec3(0.0,0.0,0.0);
    this->setCenter(newCenter);
    
    modelMatrix = glm::rotate(modelMatrix, (float)(atan2(forwardVector.y, forwardVector.x) - PI/2), UP); //Model is already rotated a quarter revolution

    
    steering = 0.0;
    accelInput = 0.0;
    
    hasCamera = false;
    
    double angle = atan2(forwardVector.y, forwardVector.x) - PI/2;

    if (LINE_COUNT == 1) {
        lines.push_back(DetectLine(center, angle, (LINE_START + LINE_END)/2, 10.0));
    } else {    
    	double increment = (LINE_END - LINE_START) / (LINE_COUNT - 1);
    	for(int i = 0; i < LINE_COUNT; i++) {
            double k = LINE_START + (increment * i);
        
            lines.push_back(DetectLine(center, angle, k, 10.0));
        }
    }
    
    organism = NULL;
}

Car::Car(glm::vec3 newCenter, glm::vec3 forward, NEAT::Organism* newOrganism) {
    if (Car::carCount >= MAX_CARS) {
        printf("Too many cars!\n");
        throw std::out_of_range("Max car limit exceeded.");
    }
    
    index = Car::carCount++;
    /* Car::carList.push_back(this); */
    
    /* printf("Making car %i\n", index); */
    
	pointDist = 0;
	stuckTimer = 0.0;
    nextPoint = 1;

    forwardVector = forward;    
    speed = 0.0;
    acceleration = 0.0;
        
    modelMatrix = glm::mat4(1.0);
    
    center = glm::vec3(0.0,0.0,0.0);
    this->setCenter(newCenter);
    
    modelMatrix = glm::rotate(modelMatrix, (float)(atan2(forwardVector.y, forwardVector.x) - PI/2), UP); //Model is already rotated a quarter revolution

    
    steering = 0.0;
    accelInput = 0.0;
    
    hasCamera = false;
    
    double angle = atan2(forwardVector.y, forwardVector.x) - PI/2;

    if (LINE_COUNT == 1) {
        lines.push_back(DetectLine(center, angle, (LINE_START + LINE_END)/2, 10.0));
    } else {    
    	double increment = (LINE_END - LINE_START) / (LINE_COUNT - 1);
    	for(int i = 0; i < LINE_COUNT; i++) {
            double k = LINE_START + (increment * i);
        
            lines.push_back(DetectLine(center, angle, k, 10.0));
        }
    }
    
    organism = newOrganism;
}

Car::~Car() {
    /* printf("destroying car %i\n", index); */
	/* Car::carList.at(index) = NULL; */
    Car::carCount--;
    if (index == selected) {
        selected = -1;
    }
}

void Car::setWatch(Car& car) {
    if (car.hasCamera != true) {
        for (int i = 0;i < Car::carCount;i++) {
            Car::carList.at(i)->hasCamera = false;
        }
    
        car.hasCamera = true;
    }
}

void Car::setCamera(glm::mat4 newCamera) {
    Car::Camera = newCamera;
}

glm::mat4 Car::getCamera() {
    return Car::Camera;
}

std::vector<Car*> Car::getCarList() {
    return Car::carList;
}

void Car::setCenter(glm::vec3 newCenter) { 
    glm::vec3 change = newCenter - center;
    
    modelMatrix = glm::translate(modelMatrix, change);
    
    center = newCenter;
}

glm::vec3 Car::getCenter() {
    return center;
}

void Car::input(double accel, double steer) {
    if (accel > 1.0) {
        accelInput = 1.0;
    } else if (accel < -1.0) {
        accelInput = -1.0;
    } else {
        accelInput = accel;
    }
    
    if (steer > 1.0) {
        steering = 1.0;
    } else if (steer < -1.0) {
        steering = -1.0;
    } else {
        steering = steer;
    }
}

void Car::inputAccel(double accel) {
    this->input(accel, steering);
}

void Car::inputSteer(double steer) {
    this->input(accelInput, steer);
}

void Car::update(double deltaTime, std::vector<Wall> walls) {
	deltaTime *= speedMulti;

    //acceleration
    acceleration = accelInput * ACCEL_FACTOR;
    
    //steering
    float steerAngle = steering * STEER_FACTOR * deltaTime;
    steerAngle *= speed / (MAX_SPEED);
    forwardVector = glm::rotate(forwardVector, steerAngle, UP);
    
    //speed
    double localSpeed = fabs(speed);

    double backAccel = speed * speed * BACK_FRICTION;

    speed += (fabs(backAccel * deltaTime) > localSpeed ? -speed : -backAccel * deltaTime);
   
    if (localSpeed < MAX_SPEED * speedMulti) {
        speed += acceleration * deltaTime;
    }
    
    glm::mat4 oldModel = modelMatrix;
    glm::vec3 oldCenter = center;
    
    //movement
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0,speed * deltaTime,0.0));
    modelMatrix = glm::rotate(modelMatrix, steerAngle, UP);
    
    
    center = glm::vec3(modelMatrix * glm::vec4(0.0,0.0,0.0,1.0));
    //center *= CAR_SCALE_FACTOR;
    
    //colision
    bool collided = false;
    for (std::vector<Wall>::iterator it = walls.begin(); it != walls.end(); it++) {
        if (collisionCarWall(*this, *it, modelMatrix)) {
            modelMatrix = oldModel;
            center = oldCenter;
            speed = 0;
            collided = true;
            break;
        }
    }
    
    if (!collided) {
        for (std::vector<DetectLine>::iterator it = lines.begin(); it != lines.end(); it++) {
            it->setCenter(center);
            it->setAngle(steerAngle + it->getAngle());
        }
    }
    
    if (hasCamera) {
        glm::vec3 cameraCenter = center + glm::vec3(0.0,0.0,1.0);
        Car::Camera = glm::lookAt(cameraCenter, center, glm::vec3(0.0,1.0,0.0));
    }
}

glm::mat4 Car::getMatrix() {
    return modelMatrix;
}

std::vector<DetectLine> Car::getLineList() {
    return lines;
}

void Car::setOrganism(NEAT::Organism* newOrganism) {
    organism = newOrganism;
}

NEAT::Organism* Car::getOrganism() {
    return organism;
}

void Car::setSpeedMulti(double multi) {
	if (multi > 0) {
	speedMulti = multi;
	}	
}

double Car::getSpeedMulti() {
	return speedMulti;
}

void Car::setSelected(int car) {
    if (car >= 0 && car < carCount) {
        selected = car;
    }
}

int Car::getSelected() {
    if (selected >= 0 && selected < carCount) {
        return selected;
    } else {
        return -1;
    }
}

int Car::getCount() {
    return carCount;
}
