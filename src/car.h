#ifndef CAR_H
#define CAR_H

class Wall;

#include <glm/glm.hpp>
#include <vector>

#include "neat.h"
#include "organism.h"

#include "detectLine.h"

//one unit is 1/50 of the screen
const double ACCEL_FACTOR = 5.0; //units/sec^2
const double MAX_SPEED = 10.0; //units/sec
const double BACK_FRICTION = 0.02; //is multiplied by the current speed squared
const double STEER_FACTOR = 3.14 / 2; //radians/sec

const int MAX_CARS = 50;
const int CAR_SCALE_FACTOR = 1;

class Car {
public:
    
	double pointDist;
	int stuckTimer;
    //center, front vector
    Car(glm::vec3 center, glm::vec3 forward);
    Car(glm::vec3 center, glm::vec3 forward, NEAT::Organism* organism);
    ~Car();
    
    //updates camera to be over the car
    static void setWatch(Car& car);
    //set the camera for watching the car
    static void setCamera(glm::mat4 newCamera);
    //returns the camera matrix;
    static glm::mat4 getCamera();
    //returns a pointer to the list of cars
    static std::vector<Car*> getCarList();
    
    //returns the center postition
    void setCenter(glm::vec3 newCenter);
    //sets the center position
    glm::vec3 getCenter();
    //accepts input for movement
    void input(double accel, double steer);
    void inputAccel(double accel);
    void inputSteer(double steer);
    //moves car based on deltaTime
    void update(double deltaTime, std::vector<Wall> walls);
    //renders the car relitive the Camera
    
    glm::mat4 getMatrix();
    
    void setOrganism(NEAT::Organism* newOrganism);
    NEAT::Organism* getOrganism();
    
    std::vector<DetectLine> getLineList();
    
	static void setSpeedMulti(double multi);
	static double getSpeedMulti();

    static void setSelected(int car);
    static int getSelected();
    static int getCount();
    
private:
    double acceleration;
    double speed;
    
    double steering;
    double accelInput;

    glm::vec3 forwardVector;
    glm::vec3 center;
    
    bool hasCamera;
    int index;
    
    glm::mat4 modelMatrix;
    
    std::vector<DetectLine> lines;
    
    NEAT::Organism* organism;
    
    static std::vector<Car*> carList;
    static int carCount;
    static int selected;
    
    static glm::mat4 Camera;
	static double speedMulti;
};
    
#endif
