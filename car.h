#ifndef CAR_H
#define CAR_H
#include <glm/glm.hpp>

//one unit is 1/200 of the screen
const float ACCEL_FACTOR = 0.2f; //units/sec^2
const float STEER_FACTOR = 2.0f; //degrees/sec
const float MAX_SPEED = 0.5f; //units/sec
const float BACK_FRICTION = 0.5f; //is multiplied by the current speed

const glm::vec3 UP = glm::vec3(0.0f,0.0f,1.0f);

const int MAX_CARS = 50;
const int SCALE_FACTOR = 5;

class Car {
public:
    
    //center, front vector
    Car(glm::vec3 center, glm::vec3 forward);
    ~Car();
    
    //updates camera to be over the car
    static void setWatch(Car* car);
    //set the camera for watching the car
    static void setCamera(glm::mat4 newCamera);
    //returns a pointer to the camera matrix;
    static glm::mat4 getCamera();
    //sets the projection matrix
    static void setProjection(glm::mat4 ProjectionMatrix);
    
    //returns the center postition
    void setCenter(glm::vec3 newCenter);
    //sets the center position
    glm::vec3 getCenter();
    //accepts input for movement
    void input(float accel, float steer);
    void inputAccel(float accel);
    void inputSteer(float steer);
    //moves car based on deltaTime
    glm::mat4 update(float deltaTime);
    //renders the car relitive the Camera
    void render();
    
    
private:
    float acceleration;
    float speed;
    
    float steering;
    float accelInput;

    glm::vec3 forwardVector;
    glm::vec3 center;
    
    bool hasCamera;
    int index;
    
    glm::mat4 modelMatrix;
    
    static Car* carList[MAX_CARS];
    static int carCount;
    
    static glm::mat4 Camera;
    static glm::mat4 Projection;
};
    
#endif
