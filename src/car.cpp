#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "car.h"
const glm::vec3 UP = glm::vec3(0.0f,0.0f,1.0f);

Car* Car::carList[MAX_CARS] = {NULL};
int Car::carCount = 0;

glm::mat4 Car::Camera = glm::mat4(1.0f);
glm::mat4 Car::Projection = glm::mat4(1.0f);

Car::Car(glm::vec3 center, glm::vec3 forward) {
    if (Car::carCount >= MAX_CARS) {
        printf("Too many cars!\n");
        throw std::out_of_range("Max car limit exceeded.");
    }
    
    index = Car::carCount++;
    Car::carList[index] = this;
    
    printf("Making car %i\n", index);
    
    forwardVector = forward;
    this->center = center;
    
    speed = 0.0f;
    acceleration = 0.0f;
    
    modelMatrix = glm::mat4(1.0f);
    
    steering = 0.0f;
    accelInput = 0.0f;
    
    hasCamera = false;
}

Car::~Car() {
    Car::carList[index] = NULL;
    printf("destroying car %i\n", index);
    if (index != Car::carCount) {
        for (int i=index+1; i < Car::carCount;i++) {
            Car::carList[i-1] = Car::carList[i];
            Car::carList[i] = NULL;
        }
    }
    
    Car::carCount--;
}

void Car::setWatch(Car* car) {
    if (car->hasCamera != true) {
        for (int i = 0;i < Car::carCount;i++) {
            if (Car::carList[i] != NULL) {
                Car::carList[i]->hasCamera = false;
            }
        }
    
        car->hasCamera = true;
    }
}

void Car::setCamera(glm::mat4 newCamera) {
    Car::Camera = newCamera;
}

glm::mat4 Car::getCamera() {
    return Car::Camera;
}

void Car::setProjection(glm::mat4 ProjectionMatrix) {
    Projection = ProjectionMatrix;
}

void Car::setCenter(glm::vec3 newCenter) { 
    center = newCenter;
}

glm::vec3 Car::getCenter() {
    return center;
}

void Car::input(float accel, float steer) {
    if (accel > 1.0f) {
        accelInput = 1.0f;
    } else if (accel < -1.0f) {
        accelInput = -1.0f;
    } else {
        accelInput = accel;
    }
    
    if (steer > 1.0f) {
        steering = 1.0f;
    } else if (steer < -1.0f) {
        steering = -1.0f;
    } else {
        steering = steer;
    }
}

void Car::inputAccel(float accel) {
    this->input(accel, steering);
}

void Car::inputSteer(float steer) {
    this->input(accelInput, steer);
}

glm::mat4 Car::update(float deltaTime) {
    //acceleration
    acceleration = accelInput * ACCEL_FACTOR;
    
    //steering
    float steerAngle = steering * STEER_FACTOR;
    steerAngle *= speed / MAX_SPEED;
    forwardVector = glm::rotate(forwardVector, steerAngle, UP);
    
    //speed
    float localSpeed = (float)fabs(speed);

    float backAccel = -speed * BACK_FRICTION;
    if (localSpeed < 0.05f) {
        backAccel *= 100.0f;
    }

    speed += (fabs(backAccel) > localSpeed ? -speed : backAccel) * deltaTime;
    
    if (localSpeed < MAX_SPEED) {
        speed += acceleration * deltaTime;
    }
    
    //printf("speed: %f\n", localSpeed);
    
    //movement
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f,speed,0.0f));
    modelMatrix = glm::rotate(modelMatrix, steerAngle, UP);
    
    center = glm::vec3(modelMatrix * glm::vec4(0.0f,0.0f,0.0f,1.0f));
    center *= SCALE_FACTOR;
    
    if (hasCamera) {
        glm::vec3 cameraCenter = center + glm::vec3(0.0f,0.0f,1.0f);
        Car::Camera = glm::lookAt(cameraCenter, center, glm::vec3(0.0f,1.0f,0.0f));
    }

    return modelMatrix;
}