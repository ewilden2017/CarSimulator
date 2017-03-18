#include "simulation.h"

#include <vector>
#include <map>
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
#include "render.h"

//TODO: move constants to a global class
const int CAR_START = 0;
const int CAR_LENGTH = 2*3;
const int WALL_START = CAR_START + CAR_LENGTH;
const int WALL_LENGTH = 2*3;
const int PATH_START = WALL_START + WALL_LENGTH;
const int PATH_LENGTH = 2;

const glm::vec3 NODE_OFFSET = glm::vec3 (30.0, -25.0, 0.0);

const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);
const glm::vec3 CAR_COLOR = glm::vec3(0.0,0.8,0.9);
const glm::vec3 CAR_COLOR_SELECTED = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 WALL_COLOR = glm::vec3(1.0,0.0,0.0);
const glm::vec3 LINE_COLOR = glm::vec3(0.1,0.1,0.1);
const glm::mat4 SCALE_MATRIX = glm::scale(glm::mat4(1.0), glm::vec3(5.0,5.0,5.0));

void carSimulation(std::vector<Car*> cars, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    
    glm::mat4 Projection = glm::ortho(-50.0,50.0,-50.0,50.0,0.0,100.0); // In world coordinates

	int nextPoint = 1;

	int nbFrames = 0;

    double StartTime = glfwGetTime();
    double time = 0;
    
    double frameLastTime = StartTime;
    double oldTime = StartTime;
    
	printf("starting\n");
    Car::setSelected(0);
    while(cars.size() > 0 && time < 60 * 1 / Car::getSpeedMulti() && glfwWindowShouldClose(window) == 0) {
        time = glfwGetTime() - StartTime;
        
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;        
        if ( currentTime - frameLastTime >= 1.0 ) {
            // printf and reset timer
            /* printf("%f ms/frame\n", 1000.0/double(nbFrames)); */
            nbFrames = 0;
            frameLastTime += 1.0;
        }
        
        double now = glfwGetTime();
        double deltaTime = now - oldTime;
        oldTime = now;
        
        glClear( GL_COLOR_BUFFER_BIT );
        //start drawing
        for(std::vector<Car*>::iterator it = cars.begin(); it != cars.end();) {
			int increment = 1;
            (*it)->update(deltaTime,*walls);

			double input[LINE_COUNT + 1]; //For biasing
			input[0] = 1.0;
            
            glm::mat4 carMatrix = Projection * Car::getCamera() * (*it)->getMatrix();
           
            glm::vec3 color;

            if (Car::getSelected() >= 0 && (*it) == cars.at(Car::getSelected())) {
                color = CAR_COLOR_SELECTED;
            } else {
                color = CAR_COLOR;
            }
			Render::renderTri(CAR_START, CAR_LENGTH, carMatrix, color);
            
            std::vector<DetectLine> lines = (*it)->getLineList();
			int i = 0;
            for (std::vector<DetectLine>::iterator lIt = lines.begin(); lIt != lines.end(); lIt++) {
				Render::renderLine(PATH_START, PATH_LENGTH, Projection * Car::getCamera() * lIt->getMatrix(), LINE_COLOR);
                
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
                
                /* if (it == cars.begin()) { */
                /*     printf("Closest:%f\n",closest); */
                /* } */
                
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
			Render::renderTri(WALL_START, WALL_LENGTH, wallMatrix, WALL_COLOR);
        }

        std::vector<glm::vec3> neurons;
        if (Car::getSelected() >= 0 && Car::getSelected() < cars.size()) {
            NEAT::Organism* org = cars.at(Car::getSelected())->getOrganism();
            if (org) {
                NEAT::Network* network = org->net;

                std::map<int, glm::vec3> nodes;

                for (int i = 0; i < network->inputs.size(); i++) {
                    NEAT::NNode* node = network->inputs.at(network->inputs.size() - 1 - i);
                    glm::vec3 position = glm::vec3(i, 0.0f, 0.0f) + NODE_OFFSET;
                    nodes.insert(std::pair<int, glm::vec3> (node->node_id, position));
                    glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position);

                    glm::vec3 renderColor;
                   

                    //Bias color
                    if (i == network->inputs.size() - 1) {
                        renderColor = glm::vec3(0.0, 0.0, 1.0);
                    } else {
                        if (node->activation > 0) {
                            float color = node->activation;
                            color = color < 0.0 ? 0.0 : 1 - color;
                            color = color < 0.1 ? color : color - 0.1;

                            renderColor = glm::vec3(0.1, color + 0.1, 0.1);
                        } else {
                            float color = node->activation * -1;
                            color = color < 0.0 ? 0.0 : 1 - color;
                            color = color < 0.1 ? color : color - 0.1;

                            renderColor = glm::vec3(color + 0.1,  0.1, 0.1);
                        }
                    }
                    Render::renderPoint(pointMatrix, renderColor, 5);
                }

                for (int i = 0; i < network->outputs.size(); i++) {
                    NEAT::NNode* node = network->outputs.at(i);
                    glm::vec3 position = glm::vec3(i, -1 * (network->max_depth() + 5), 0.0f) + NODE_OFFSET;
                    nodes.insert(std::pair<int, glm::vec3> (node->node_id, position));
                    glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position);

                    glm::vec3 renderColor;
                    if (node->activation > 0) {
                        float color = node->activation;
                        color = color < 0.0 ? 0.0 : 1 - color;
                        color = color < 0.1 ? color : color - 0.1;

                        renderColor = glm::vec3(0.1, color + 0.1, 0.1);
                    } else {
                        float color = node->activation * -1;
                        color = color < 0.0 ? 0.0 : 1 - color;
                        color = color < 0.1 ? color : color - 0.1;

                            renderColor = glm::vec3(color + 0.1,  0.1, 0.1);
                    }

                    Render::renderPoint(pointMatrix, renderColor, 5);
                }

                for (int i = network->inputs.size(); i < network->all_nodes.size() - network->outputs.size(); i++) {
                    NEAT::NNode* node = network->all_nodes.at(i);
                    glm::vec3 position = glm::vec3(i, node->depth(0, network), 0.0) + NODE_OFFSET;
                    nodes.insert(std::pair<int, glm::vec3> (node->node_id, position));
                    glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position + glm::vec3(-30.0,0.0,0.0));

                    glm::vec3 renderColor;
                    if (node->activation > 0) {
                        float color = node->activation;
                        color = color < 0.0 ? 0.0 : 1 - color;
                        color = color < 0.1 ? color : color - 0.1;

                        renderColor = glm::vec3(0.1, color + 0.1, 0.1);
                    } else {
                        float color = node->activation * -1;
                        color = color < 0.0 ? 0.0 : 1 - color;
                        color = color < 0.1 ? color : color - 0.1;

                            renderColor = glm::vec3(color + 0.1,  0.1, 0.1);
                    }

                    Render::renderPoint(pointMatrix, renderColor, 5);
                }

                for (std::vector<NEAT::NNode*>::iterator it = network->all_nodes.begin(); it != network->all_nodes.end(); it++) {
                    std::vector<NEAT::Link*> outgoing = (*it)->outgoing;
                    for (std::vector<NEAT::Link*>::iterator linkIt = outgoing.begin(); linkIt != outgoing.end(); linkIt ++) {
                        NEAT::Link* link = (*linkIt);
                        glm::vec3 inPoint = nodes.at(link->in_node->node_id) + glm::vec3(0.0, 0.0, -5.0);
                        glm::vec3 outPoint = nodes.at(link->out_node->node_id) + glm::vec3(0.0, 0.0, -5.0);
                        glm::vec3 line = outPoint - inPoint;

                        float inValue = link->in_node->activation;
                        inValue = inValue < 0.0 ? 0.0 : 1 - inValue; 
                        inValue = inValue < 0.2 ? inValue : inValue - 0.2;

                        glm::vec3 color;
                        if (link->weight > 0) {
                            color = glm::vec3(0.2, inValue * link->weight + 0.2, 0.2);
                        } else {
                            color = glm::vec3(-1 * inValue * link->weight + 0.2, 0.2, 0.2);
                        }

                        double angle = atan2(line.y, line.x) - PI/2;

                        glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(0.0,glm::distance(inPoint, outPoint),0.0));
                        glm::mat4 translation = glm::translate(glm::mat4(), inPoint);
                        glm::mat4 rotation = glm::rotate(glm::mat4(), (float) angle, UP);
                        glm::mat4 model = translation * rotation * scale;

                        glm::mat4 linkMatrix = Projection * model;

                        Render::renderLine(PATH_START, PATH_LENGTH, linkMatrix, color);
 
                    } 
                }

            }
        }
    

        
        //finish drawing
        glfwSwapBuffers(window);
        
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glfwPollEvents(); 
    }
	//Done, clean up
	for(std::vector<Car*>::iterator it = cars.begin(); it != cars.end();it++) {
		delete (*it);
	}
	cars.clear();
}
