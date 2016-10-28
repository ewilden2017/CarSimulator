#include "simulation.h"

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"
#include "car.h"
#include "collision.h"
#include "wall.h"
#include "wallLoader.h"

#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"


const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);
const glm::vec3 CAR_COLOR = glm::vec3(0.0,0.8,0.9);
const glm::vec3 WALL_COLOR = glm::vec3(1.0,0.0,0.0);
const glm::vec3 LINE_COLOR = glm::vec3(0.1,0.1,0.1);
const glm::mat4 SCALE_MATRIX = glm::scale(glm::mat4(1.0), glm::vec3(5.0,5.0,5.0));

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

void carSimulation(std::vector<Car*> cars, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    
	//TODO: Move this junk to main, or a common class
    GLuint programID = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ColorID = glGetUniformLocation(programID, "inColor");

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
    
    glm::mat4 Projection = glm::ortho(-50.0,50.0,-50.0,50.0,0.0,100.0); // In world coordinates

	int nextPoint = 1;

	int nbFrames = 0;

    double StartTime = glfwGetTime();
    double time = StartTime;
    
    double frameLastTime = StartTime;
    double oldTime = StartTime;
    
	printf("starting\n");
    while(cars.size() > 0 && time < 60 * 1) {
        time = glfwGetTime() - StartTime;
        
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
        
        //test position
        /* glm::vec3 distance = myCar.getCenter() - path.at(nextPoint); */
        /* printf("Next: %i (%f)\n", nextPoint, glm::dot(distance, distance)); */
        /* if(glm::dot(distance, distance) < distances.at(nextPoint)*distances.at(nextPoint)) { */
            /* nextPoint++; */
            /* if (nextPoint > path.size() - 1) { */
                /* nextPoint = 0; */
            /* } */
        /* } */
        
        /* std::vector<Car*> carList = Car::getCarList(); */
        
        glClear( GL_COLOR_BUFFER_BIT );
        //start drawing
        for(std::vector<Car*>::iterator it = cars.begin(); it != cars.end();) {
			int increment = 1;
            (*it)->update(deltaTime,*walls);

			double input[LINE_COUNT + 1]; //For biasing
			input[0] = 1.0;
            
            glm::mat4 carMatrix = Projection * Car::getCamera() * (*it)->getMatrix();
            render(programID, MatrixID, ColorID, carbuffer, carVAO, carMatrix, CAR_COLOR);
            
            std::vector<DetectLine> lines = (*it)->getLineList();
			int i = 0;
            for (std::vector<DetectLine>::iterator lIt = lines.begin(); lIt != lines.end(); lIt++) {
                renderLine(programID, MatrixID, ColorID, linebuffer, lineVAO, Projection * Car::getCamera() * lIt->getMatrix(), LINE_COLOR);
                
                double closest = -1;
                for (std::vector<Wall>::iterator wIt = walls->begin(); wIt != walls->end(); wIt++) {
                    glm::mat4 wallMatrix = wIt->getMatrix();
                    glm::vec3 wallB = glm::vec3(wallMatrix * glm::vec4(1.0,0.0,0.0,1.0));
                    glm::vec3 wallA = glm::vec3(wallMatrix * glm::vec4(-1.0,0.0,0.0,1.0));
                    //printf("A:%s, B:%s\n", glm::to_string(wallA).c_str(), glm::to_string(wallB).c_str());
                    double out = 0;
                    if (collisionLineLine(lIt->getCenter(), lIt->getEnd(), wallA , wallB, &out) == true) {
                        if (closest == -1 || out < closest) {
                            closest = out;
                        }
                    }
                }
				input[i+1] = closest;
				i++;
                /* printf("Closest:%f\n",closest); */
                
                lIt->setDistance(closest);
            }

			NEAT::Organism* org = (*it)->getOrganism();
			if (org) {
				NEAT::Network* net = org->net;
				net->load_sensors(input);

				if (!(net->activate())) return;
			
				std::vector<NEAT::NNode*>::iterator out_iter = net->outputs.begin();
				double steering = (*out_iter)->activation;
				out_iter++;
				double accel = (*out_iter)->activation;

				(*it)->input(accel, steering);

				glm::vec3 distance = (*it)->getCenter() - path->at(nextPoint);
				glm::vec3 totalDistance = path->at(nextPoint-1) - path->at(nextPoint);
				
				double newFit = (*it)->pointDist + glm::dot(totalDistance, totalDistance)- glm::dot(distance, distance); //remember, distance is squared
				double deltaFit = org->fitness - newFit;
				if (deltaFit < 1 && deltaFit > -1) {
					/* printf("Stuck: %i\n", (*it)->stuckTimer); */
					(*it)->stuckTimer += 1;

					if ((*it)->stuckTimer > 500) {
						delete (*it);
						it = cars.erase(it);
						increment = 0;
					}
				} else {
					(*it)->stuckTimer = 0;
				}

				org->fitness = newFit;

				if(glm::dot(distance, distance) < distances->at(nextPoint)*distances->at(nextPoint)) {
					(*it)->pointDist += glm::dot(distance, distance);
					nextPoint++;
					if (nextPoint > path->size() - 1) {
						nextPoint = 0;
					}
				}
 
				/* printf("Fitness: %f\n", org->fitness); */
			}

			if (increment) it++;
        }
        
        for(int i = 0; i < walls->size(); i++) {
            glm::mat4 wallMatrix = Projection * Car::getCamera() * walls->at(i).getMatrix();
            render(programID, MatrixID, ColorID, wallbuffer, wallVAO, wallMatrix, WALL_COLOR);
        }
        
        //finish drawing
        glfwSwapBuffers(window);
        
        glClear( GL_COLOR_BUFFER_BIT );
        glfwPollEvents(); 
    }
	//Done, clean up
	for(std::vector<Car*>::iterator it = cars.begin(); it != cars.end();it++) {
		delete (*it);
	}
	cars.clear();
}