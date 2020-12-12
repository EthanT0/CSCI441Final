//
// Created by ethan on 11/23/20.
//

#ifndef A5_PARTICLE_H
#define A5_PARTICLE_H


#include <CSCI441/ShaderUtils.hpp>

#include <glm/glm.hpp>


class Particle {
    glm::vec3 position;
    glm::vec3 initialPosition;
    glm::vec3 velocity;

    float aliveTime;
public:
    Particle(glm::vec3 initialPos, glm::vec3 initialVelocity);
    GLfloat distanceToCamera(glm::mat4 & modelMat, glm::vec3 & cameraPos, glm::vec3 forwardVector);

    glm::vec3 update(float timeStep, glm::vec3 wrapPosition = glm::vec3(0), glm::vec3 wrapSize = glm::vec3(20));

    glm::vec3 getPosition();

    void resetPosition();
};


#endif //A5_PARTICLE_H
