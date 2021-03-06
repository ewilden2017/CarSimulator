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

const glm::vec3 NODE_OFFSET = glm::vec3 (20.0, -25.0, 0.0);
const double NODE_SCALE = 2.0;

const glm::vec3 UP = glm::vec3(0.0,0.0,1.0);
const glm::vec3 CAR_COLOR = glm::vec3(0.0,0.8,0.9);
const glm::vec3 CAR_COLOR_SELECTED = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 WALL_COLOR = glm::vec3(1.0,0.0,0.0);
const glm::vec3 LINE_COLOR = glm::vec3(0.1,0.1,0.1);
const glm::mat4 SCALE_MATRIX = glm::scale(glm::mat4(1.0), glm::vec3(5.0,5.0,5.0));

const double CAMERA_PAN_SPEED = 15.0;
const double CAMERA_ZOOM_SPEED = 1.0;

void carSimulation(std::vector<Car*> cars, std::vector<Wall>* walls, std::vector<glm::vec3>* path, std::vector<double>* distances, GLFWwindow* window) {
    
    glm::mat4 Projection = glm::ortho(-50.0,50.0,-50.0,50.0,0.0,100.0); // In world coordinates

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
       
        //Move the camera
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glm::mat4 camera = glm::translate(Car::getCamera(), glm::vec3(0.0, -CAMERA_PAN_SPEED * deltaTime, 0.0));
            Car::setCamera(camera);
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm::mat4 camera = glm::translate(Car::getCamera(), glm::vec3(0.0, CAMERA_PAN_SPEED * deltaTime, 0.0));
            Car::setCamera(camera);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::mat4 camera = glm::translate(Car::getCamera(), glm::vec3(CAMERA_PAN_SPEED * deltaTime, 0.0, 0.0));
            Car::setCamera(camera);
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::mat4 camera = glm::translate(Car::getCamera(), glm::vec3(-CAMERA_PAN_SPEED * deltaTime, 0.0, 0.0));
            Car::setCamera(camera);
        } 
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            glm::mat4 camera = glm::scale(Car::getCamera(), glm::vec3(1+(CAMERA_ZOOM_SPEED * deltaTime)));
            Car::setCamera(camera);
        } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            glm::mat4 camera = glm::scale(Car::getCamera(), glm::vec3(1+(-CAMERA_ZOOM_SPEED * deltaTime)));
            Car::setCamera(camera);
        }

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
                if (closest == -1) {
                    input[i+1] = 1;
                }
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
                steering = (steering * 2) - 1; //scale from -1 to 1
				out_iter++;
				double accel = (*out_iter)->activation;
                accel = (accel * 2) - 1;

				(*it)->input(accel, steering);

                int nextPoint = (*it)->nextPoint;
                int lastPoint = nextPoint - 1;
                lastPoint = lastPoint < 0 ? path->size() - 1 : lastPoint;

				glm::vec3 distance = (*it)->getCenter() - path->at(nextPoint);
				glm::vec3 totalDistance = path->at(lastPoint) - path->at(nextPoint);

                double segmentFit = glm::dot(totalDistance, totalDistance)- glm::dot(distance, distance);

                int lastTwoPoint = lastPoint - 1;
                lastTwoPoint = lastTwoPoint < 0 ? path->size() - 1 : lastTwoPoint;

                glm::vec3 prevTotal = path->at(lastTwoPoint) - path->at(lastPoint);
                glm::vec3 prevDistance = (*it)->getCenter() - path->at(lastTwoPoint);
                glm::vec3 lastDistance = (*it)->getCenter() - path->at(lastPoint);

                //If car is behind point, move it backwards on path
                if (glm::dot(prevTotal, prevTotal) - glm::dot(prevDistance, prevDistance) > segmentFit && glm::dot(lastDistance, lastDistance) > distances->at(lastPoint)*distances->at(lastPoint)) {
                    int newPoint = lastPoint;
                    int newLast = lastPoint - 1;
                    newLast = newLast < 1 ? path->size() - 1 : newLast;

                    (*it)->nextPoint = newPoint;
                    nextPoint = newPoint;
                    lastPoint = newLast;

                    totalDistance = path->at(lastPoint) - path->at(nextPoint);
                    distance = (*it)->getCenter() - path->at(nextPoint);
                    segmentFit = glm::dot(totalDistance, totalDistance)- glm::dot(distance, distance);

                    (*it)->pointDist -= glm::dot(totalDistance, totalDistance);
                    segmentFit = glm::dot(totalDistance, totalDistance) - glm::dot(distance, distance);
                }

               
				double newFit = (*it)->pointDist + segmentFit; //remember, distance is squared
				double deltaFit = org->fitness - newFit;
				if (deltaFit < 1 && deltaFit > -1) {
					/* printf("Stuck: %i\n", (*it)->stuckTimer); */
					(*it)->stuckTimer += 1 * deltaTime * Car::getSpeedMulti();

					if ((*it)->stuckTimer > 10.0) {
						delete (*it);
						it = cars.erase(it);
						increment = 0;
					}
				} else {
					(*it)->stuckTimer = 0.0;
				}

				org->fitness = newFit;

                int checkPoint = nextPoint + 1;
                checkPoint = checkPoint > path->size() - 1 ? 1 : checkPoint;
                glm::vec3 nextDistance = (*it)->getCenter() - path->at(checkPoint);
				glm::vec3 nextTotal = path->at(nextPoint) - path->at(checkPoint);



				if(glm::dot(nextTotal, nextTotal) - glm::dot(nextDistance, nextDistance) > 0 && glm::dot(distance, distance) < distances->at(nextPoint)*distances->at(nextPoint)) {
					(*it)->pointDist += glm::dot(totalDistance, totalDistance);
					(*it)->nextPoint++;
					if ((*it)->nextPoint > path->size() - 1) {
						(*it)->nextPoint = 1;
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
                //Draw the next point
                int nextPoint = cars.at(Car::getSelected())->nextPoint;
                glm::mat4 model = glm::translate(glm::mat4(), path->at(nextPoint));
                glm::mat4 MVP = Projection * Car::getCamera() * model;
                Render::renderPoint(MVP, glm::vec3(0.0, 1.0, 1.0), 5);

                NEAT::Network* network = org->net;

                std::map<NEAT::NNode*, glm::vec3> nodes;

                for (int i = 0; i < network->inputs.size(); i++) {
                    NEAT::NNode* node = network->inputs.at(network->inputs.size() - 1 - i);
                    glm::vec3 position = glm::vec3(i * NODE_SCALE, 0.0f, 0.0f) + NODE_OFFSET;
                    nodes.insert(std::pair<NEAT::NNode*, glm::vec3> (node, position));
                    glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position);

                    glm::vec3 renderColor;
                   

                    //Bias color
                    if (i == network->inputs.size() - 1) {
                        renderColor = glm::vec3(0.0, 0.0, 1.0);
                    } else {
                        double value = node->get_active_out(); //already scaled from 0 to 1
                        if (value > 0) {
                            float color = value * 0.8;
                            /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                            renderColor = glm::vec3(0.2, color + 0.2, 0.2);
                        } else {
                            float color = value * -0.8;
                            /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                            renderColor = glm::vec3(color + 0.2,  0.2, 0.2);
                        }
                    }
                    Render::renderPoint(pointMatrix, renderColor, 5);
                }

                for (int i = 0; i < network->outputs.size(); i++) {
                    NEAT::NNode* node = network->outputs.at(i);
                    glm::vec3 position = glm::vec3(i * NODE_SCALE, -1 * (network->max_depth() + 1) * NODE_SCALE, 0.0f) + NODE_OFFSET;
                    nodes.insert(std::pair<NEAT::NNode*, glm::vec3> (node, position));
                    glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position);

                    glm::vec3 renderColor;
                    double value = (node->get_active_out() * 2) - 1;
                    if (value > 0) {
                        float color = value * 0.8;
                        /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                        renderColor = glm::vec3(0.2, color + 0.2, 0.2);
                    } else {
                        float color = value * -0.8;
                        /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                            renderColor = glm::vec3(color + 0.2,  0.2, 0.2);
                    }

                    Render::renderPoint(pointMatrix, renderColor, 5);
                }

                for (int i = 0; i < network->all_nodes.size(); i++) {
                    NEAT::NNode* node = network->all_nodes.at(i);
                    if (nodes.find(node) == nodes.end()) {
                        glm::vec3 position = glm::vec3((i - network->inputs.size()) * NODE_SCALE, -1 * (node->depth(0, network)) * NODE_SCALE, 0.0) + NODE_OFFSET;
                        nodes.insert(std::pair<NEAT::NNode*, glm::vec3> (node, position));
                        glm::mat4 pointMatrix = Projection * glm::translate(glm::mat4(), position);

                        glm::vec3 renderColor;
                        double value = (node->get_active_out() * 2) - 1;
                        if (value > 0) {
                            float color = value * 0.8;
                            /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                            renderColor = glm::vec3(0.2, color + 0.2, 0.2);
                        } else {
                            float color = value * -0.8;
                            /* color = color < 0.0 ? 0.0 : 0.8 - color; */

                            renderColor = glm::vec3(color + 0.2,  0.2, 0.2);
                        }

                        Render::renderPoint(pointMatrix, renderColor, 5);
                    }
                }

                for (std::vector<NEAT::NNode*>::iterator it = network->all_nodes.begin(); it != network->all_nodes.end(); it++) {
                    std::vector<NEAT::Link*> outgoing = (*it)->outgoing;
                    for (std::vector<NEAT::Link*>::iterator linkIt = outgoing.begin(); linkIt != outgoing.end(); linkIt ++) {
                        NEAT::Link* link = (*linkIt);

                        if (nodes.find(link->in_node) != nodes.end() && nodes.find(link->out_node) != nodes.end()) {
                            glm::vec3 inPoint = nodes.at(link->in_node) + glm::vec3(0.0, 0.0, -5.0);
                            glm::vec3 outPoint = nodes.at(link->out_node) + glm::vec3(0.0, 0.0, -5.0);
                            glm::vec3 line = outPoint - inPoint;

                            float inValue = fabs(link->in_node->activation) * 0.9;
                            /* inValue = inValue < 0.0 ? 0.0 : 0.9 - inValue; */ 

                            glm::vec3 color;
                            if (link->weight > 0) {
                                color = glm::vec3(0.1, inValue * link->weight + 0.1, 0.1);
                            } else {
                                color = glm::vec3(-1 * inValue * link->weight + 0.1, 0.1, 0.1);
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
