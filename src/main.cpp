// Include standard headers
#include <stdio.h>
#include <vector>
#include <math.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"
#include "wall.h"
#include "car.h"
#include "collision.h"
#include "detectLine.h"

const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);
const glm::vec3 CAR_COLOR = glm::vec3(0.0,0.8,0.9);
const glm::vec3 WALL_COLOR = glm::vec3(1.0,0.0,0.0);
const glm::vec3 LINE_COLOR = glm::vec3(0.1,0.1,0.1);
const glm::mat4 SCALE_MATRIX = glm::scale(glm::mat4(1.0), glm::vec3(5.0,5.0,5.0));

Car* pCar = NULL;

const GLfloat carData[] = { 
    -0.5f, -1.0f, 0.0f,
    0.5f, -1.0f, 0.0f,
    -0.5f,  1.0f, 0.0f,
    0.5f, 1.0f, 0.0f,
    -0.5f, 1.0f, 0.0f,
    0.5f, -1.0f, 0.0f,
};

const GLfloat wallData[] = { 
    -1.0f, -0.2f, 0.0f,
    1.0f, -0.2f, 0.0f,
    -1.0f,  0.2f, 0.0f,
    1.0f, 0.2f, 0.0f,
    -1.0f, 0.2f, 0.0f,
    1.0f, -0.2f, 0.0f,
};

const GLfloat lineData[] = {
    0.0f , 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
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
    window = glfwCreateWindow( 1024, 768, "CarSimulator", NULL, NULL);
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
    //TODO: Make this respect size of data
    glDrawArrays(GL_TRIANGLES, 0, 2*3); // 3 indices starting at 0 -> 1 triangle
    
    glBindVertexArray(0);
    glUseProgram(0);
}

void renderLine(GLuint programID, GLuint MatrixID, GLuint ColorID, GLuint vertexbuffer, GLuint VAO, glm::mat4 MVP, glm::vec3 color) {
    glUseProgram(programID);
    
    glBindVertexArray(VAO);
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(ColorID, 1, &color[0]);
    
    glDrawArrays(GL_LINES, 0 , 2);
    
    glBindVertexArray(0);
    glUseProgram(0);
}

int main( void )
{
    Car myCar(glm::vec3(0.0,-5.0,0.0), glm::vec3(0.0,1.0,0.0));
    //Car car2(glm::vec3(0.0,5.0,0.0), glm::vec3(-1.0,1.0,0.0));
    pCar = &myCar;
    std::vector<Wall> walls;
    walls.push_back(Wall(glm::vec3(0.0,0.0,0.0), glm::vec3(15.0,5.0,0.0)));
    walls.push_back(Wall(glm::vec3(15.0,5.0,0.0), glm::vec3(30.0,12.5,0.0)));
    walls.push_back(Wall(glm::vec3(30.0,12.5,0.0), glm::vec3(35.0,10.0,0.0)));
    walls.push_back(Wall(glm::vec3(35.0,10.0,0.0), glm::vec3(25.0,6.5,0.0)));
    walls.push_back(Wall(glm::vec3(25.0,6.5,0.0), glm::vec3(0.0,0.0,0.0)));
    
    int error = init();
    if (error != 0) {
        return error;
    }
    
    // black background
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    GLuint programID = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );
    
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ColorID = glGetUniformLocation(programID, "inColor");
    
    glm::mat4 Projection = glm::ortho(-50.0,50.0,-50.0,50.0,0.0,100.0); // In world coordinates
    glm::mat4 View = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
    
    Car::setCamera(View);
    
    
    //WARNING: I don't actually understand vertex arrays
    
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
    
    GLuint linebuffer;
    GLuint lineVAO;
    
    glGenBuffers(1, &linebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, linebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineData), lineData, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    double frameLastTime = glfwGetTime();
    int nbFrames = 0;
    
    double oldTime = glfwGetTime();
    
    Car::setWatch(myCar);
    
    while(glfwWindowShouldClose(window) == 0) {
        
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;        
        if ( currentTime - frameLastTime >= 1.0 ) {
            // printf and reset timer
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            frameLastTime += 1.0;
        }
        
        double now = glfwGetTime();
        double deltaTime = now - oldTime;
        oldTime = now;
        
        std::vector<Car*> carList = Car::getCarList();
        
        glClear( GL_COLOR_BUFFER_BIT );
        //start drawing
        
        for(std::vector<Car*>::iterator it = carList.begin(); it != carList.end(); it++) {
            (*it)->update(deltaTime,walls);            
           
            
            glm::mat4 carMatrix = Projection * Car::getCamera() * (*it)->getMatrix();
            render(programID, MatrixID, ColorID, carbuffer, carVAO, carMatrix, CAR_COLOR);
            
            std::vector<DetectLine> lines = (*it)->getLineList();
            for (std::vector<DetectLine>::iterator lIt = lines.begin(); lIt != lines.end(); lIt++) {
                renderLine(programID, MatrixID, ColorID, linebuffer, lineVAO, Projection * Car::getCamera() * lIt->getMatrix(), LINE_COLOR);
                
                double closest = -1;
                for (std::vector<Wall>::iterator wIt = walls.begin(); wIt != walls.end(); wIt++) {
                    glm::mat4 wallMatrix = wIt->getMatrix();
                    glm::vec3 wallB = glm::vec3(wallMatrix * glm::vec4(1.0,0.0,0.0,1.0));
                    glm::vec3 wallA = glm::vec3(wallMatrix * glm::vec4(-1.0,0.0,0.0,1.0));
                    //printf("A:%s, B:%s\n", glm::to_string(wallA).c_str(), glm::to_string(wallB).c_str());
<<<<<<< HEAD
                    double out = 0;
                    if (collisionLineLine(lIt->getCenter(), lIt->getEnd(), wallA , wallB, &out) == true) {
                        if (closest == -1 || out < closest) {
                            closest = out;
                        }
                    }
                }
                printf("Closest:%f\n",closest);
=======
                    double *out;
                    if (collisionLineLine(lIt->getCenter(), lIt->getEnd(), wallA , wallB, out) == true) {
                        printf("Hello!\n");
                    }
                    if (closest < 0 || *out < closest) {
                        closest = *out;
                    }
                }
                
>>>>>>> b78d6fc1e7f1489da45cc750380a380b41abd915
                lIt->setDistance(closest);
                //printf("Distance:%f\n", closest);
            }
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
    glDeleteBuffers(1, &linebuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &carVAO);
    glDeleteVertexArrays(1, &wallVAO);
    glDeleteVertexArrays(1, &lineVAO);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}