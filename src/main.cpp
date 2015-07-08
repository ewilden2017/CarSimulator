// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"
#include "car.h"
#include "wall.h"

const glm::vec3 UP = glm::vec3(0.0f,0.0f,1.0f);
const glm::vec3 CAR_COLOR = glm::vec3(0.0f,0.8f,0.9f);
const glm::vec3 WALL_COLOR = glm::vec3(1.0f,0.0f,0.0f);
const glm::mat4 SCALE_MATRIX = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f,5.0f,5.0f));

Car* pCar = NULL;

const GLfloat carData[] = { 
    -0.5f, -1.0f, 0.0f,
    0.5f, -1.0f, 0.0f,
    -0.5f,  1.0f, 0.0f,
    0.5f, 1.0f, 0.0f,
    -0.5f, 1.0f, 0.0f,
    0.5f, -1.0f, 0.0f,
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            pCar->inputAccel(1);
        }
        if (key == GLFW_KEY_S) {
            pCar->inputAccel(-1);
        }
        if (key == GLFW_KEY_D) {
            pCar->inputSteer(-1);
        }
        if (key == GLFW_KEY_A) {
            pCar->inputSteer(1);
        }
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key ==GLFW_KEY_S) {
            pCar->inputAccel(0);
        }
        if (key == GLFW_KEY_D || key == GLFW_KEY_A) {
            pCar->inputSteer(0);
        }
    }
}

int init() {
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.Is your GPU OpenGL 3.3 compatable?.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, keyCallback);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    return 0;
}

void render(GLuint programID, GLuint MatrixID, GLuint ColorID, GLuint vertexbuffer, GLuint VAO, glm::mat4 MVP, glm::vec3 color) {
    glUseProgram(programID);
    
    glBindVertexArray(VAO);
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(ColorID, 1, &color[0]);
    
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 2*3); // 3 indices starting at 0 -> 1 triangle
    
    glBindVertexArray(0);
    glUseProgram(0);
}

int main( void )
{
    Car myCar(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
    Car car2(glm::vec3(0.0f,5.0f,0.0f), glm::vec3(-1.0f,1.0f,0.0f));
    pCar = &myCar;
    std::vector<Wall> walls;
    for (int i = 0; i < 10; i++) {
        walls.push_back(Wall(glm::vec3(0.0f,0.0f,0.0f), glm::vec3((float)i,10.0f,0.0f)));
    }
    
    int error = init();
    if (error != 0) {
        return error;
    }
    
    // black background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ColorID = glGetUniformLocation(programID, "inColor");
    
    glm::mat4 Projection = glm::ortho(-50.0f,50.0f,-50.0f,50.0f,0.0f,100.0f); // In world coordinates
    glm::mat4 View = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
    
    Car::setCamera(View);    
    
    GLuint carbuffer;
    GLuint carVAO;
    
    glGenBuffers(1, &carbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, carbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(carData), carData, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &carVAO);
    glBindVertexArray(carVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint wallbuffer;
    GLuint wallVAO;
    glGenBuffers(1, &wallbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, wallbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallData), wallData, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &wallVAO);
    glBindVertexArray(wallVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
    
//     Car::setWatch(&myCar);
    
    double frameLastTime = glfwGetTime();
    int nbFrames = 0;
    
    double oldTime = glfwGetTime();
    
    while(glfwWindowShouldClose(window) == 0) {
        
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;        
        if ( currentTime - frameLastTime >= 1.0 ){ // If last printf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            frameLastTime += 1.0;
        }
        
        double now = glfwGetTime();
        double deltaTime = now - oldTime;
        oldTime = now;
        
        std::vector<Car*> carList = Car::getCarList();
        
        // Our ModelViewProjection : multiplication of our 3 matrices
        //glm::mat4 carMVP = Projection * Car::getCamera() * myCar.update(deltaTime); // Remember, matrix multiplication is the other way around
        //glm::mat4 wallMVP = Projection * Car::getCamera() * walls.at(0).getMatrix();
        
        glClear( GL_COLOR_BUFFER_BIT );
        //start drawing
        
        for(int i = 0; i < carList.size(); i++) {
            glm::mat4 carMatrix = Projection * Car::getCamera() * carList.at(i)->update(deltaTime);
            render(programID, MatrixID, ColorID, carbuffer, carVAO, carMatrix, CAR_COLOR);
        }
        
        for(int i = 0; i < walls.size(); i++) {
            glm::mat4 wallMatrix = Projection * Car::getCamera() * walls.at(i).getMatrix();
            render(programID, MatrixID, ColorID, wallbuffer, wallVAO, wallMatrix, WALL_COLOR);
        }
        
        //finish drawing
        glfwSwapBuffers(window);
        
        glClear( GL_COLOR_BUFFER_BIT );
        glfwPollEvents(); 
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &carbuffer);
    glDeleteBuffers(1, &wallbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &carVAO);
    glDeleteVertexArrays(1, &wallVAO);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

