// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "car.h"

Car myCar(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));

void move(glm::mat4 camera, glm::mat4 model, glm::vec3 velocity) {
    glm::translate(model, velocity);
    glm::translate(camera, velocity);
    
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            myCar.inputAccel(1);
        } else if (key == GLFW_KEY_S) {
            myCar.inputAccel(-1);
        } else if (key == GLFW_KEY_D) {
            myCar.inputSteer(-1);
        } else if (key == GLFW_KEY_A) {
            myCar.inputSteer(1);
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key ==GLFW_KEY_S) {
            myCar.inputAccel(0);
        } else if (key == GLFW_KEY_D || key == GLFW_KEY_A) {
            myCar.inputSteer(0);
        }
    }
}

int main( void )
{
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
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
    
    // Ensure we can capture the escape key being pressed below
 //   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    //glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Or, for an ortho camera :
    glm::mat4 Projection = glm::ortho(-200.0f,200.0f,-200.0f,200.0f,0.0f,100.0f); // In world coordinates
    
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
        glm::vec3(0,0,1), // Camera is at (0,0,1), in World Space
                                       glm::vec3(0,0,0), // and looks at the origin
                                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    
    );
    
    Car::setProjection(Projection);
    Car::setCamera(View);
    
    static const GLfloat g_vertex_buffer_data[] = { 
        -0.5f, -1.0f, 0.0f,
        0.5f, -1.0f, 0.0f,
        -0.5f,  1.0f, 0.0f,
        0.5f, 1.0f, 0.0f,
        -0.5f, 1.0f, 0.0f,
        0.5f, -1.0f, 0.0f,
    };
 //   static const GLushort g_element_buffer_data[] = { 0, 1, 2 };
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    Car::setWatch(&myCar);
    
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    
    double oldTime = glfwGetTime();
    
    do {
        
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;        
        if ( currentTime - lastTime >= 1.0 ){ // If last printf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f,5.0f,0.0f));
        
        double now = glfwGetTime();
        Model *= myCar.update(now - oldTime);
        oldTime = now;
        
        printf("Camera:%s\n", glm::to_string(Car::getCamera()).c_str());
        printf("Model:%s\n", glm::to_string(Model).c_str());
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP = Projection * Car::getCamera() * Model; // Remember, matrix multiplication is the other way around
        
        
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );
        
        // Use our shader
        glUseProgram(programID);
        
        // Send our transformation to the currently bound shader, 
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        
        
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 2*3); // 3 indices starting at 0 -> 1 triangle
        
        glDisableVertexAttribArray(0);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while(glfwWindowShouldClose(window) == 0);
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

