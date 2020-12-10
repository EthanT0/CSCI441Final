//
// Created by ethan on 11/23/20.
//

#include "Particle.h"
#include <GLFW/glfw3.h>			        // include GLFW framework header


Particle::Particle(glm::vec3 initialPos, glm::vec3 initialVelocity) : position(initialPos), initialPosition(initialPos), velocity(initialVelocity){
    resetPosition();
}


GLfloat Particle::distanceToCamera(glm::mat4 & modelMat, glm::vec3 & cameraPos, glm::vec3 forwardVector){
    glm::vec3 particleWorldPos = glm::vec3(modelMat * glm::vec4(position, 1));
    GLfloat distance = dot((cameraPos - particleWorldPos), forwardVector);
    return distance;
}

glm::vec3 Particle::update(GLfloat timeStep, glm::vec3 wrapPosition, glm::vec3 wrapSize){

    aliveTime += timeStep;

    position += (velocity * 0.01f) * aliveTime;

}

void Particle::resetPosition() {
    position = initialPosition;
    aliveTime = 0;
}

glm::vec3 Particle::getPosition(){
    return position;
}