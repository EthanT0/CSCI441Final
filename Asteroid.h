//
// Created by matth on 12/8/2020.
//

#ifndef FINAL_ASTEROID_H
#define FINAL_ASTEROID_H
#include "Asteroid.h"

// include OpenGL and GLFW libraries
#include <GL/glew.h>                    // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h>			        // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Asteroid {

public:


    Asteroid();
    Asteroid(glm::vec3 initialPosition, glm::vec3 velocity, glm::vec3 rotAxis, GLfloat scale, GLfloat rotationSpeed);

    float scale;
    float rotationSpeed;
    float rotationAngle;

    glm::vec3 velocity;


    glm::vec3 translationVector;
    glm::vec3 rotationAxis;
    glm::vec3 scaleVector;

    void bounce(glm::vec3 normal, GLfloat bounciness = 1.0f);

    glm::vec3 getVelocity();

    glm::vec3 getPosition();

    glm::mat4 getTransformation();

    void update(GLfloat timeStep, glm::vec3 shipVelocity, glm::vec3 domainSize);

    GLboolean collisionTest(glm::vec3 point, GLfloat pointRadius = 0.0f);
};


#endif //FINAL_ASTEROID_H
