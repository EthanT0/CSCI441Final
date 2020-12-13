//
// Created by ethan on 12/12/20.
//

#ifndef A6_PROJECTILE_H
#define A6_PROJECTILE_H

// include OpenGL and GLFW libraries
#include <GL/glew.h>                    // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h>			        // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Projectile {

public:
    Projectile(glm::vec3 initialPosition, glm::vec3 velocity);

    glm::vec3 velocity;
    glm::vec3 position;
    glm::mat4 rotationTransform;
    GLfloat age;

    glm::vec3 getVelocity();

    glm::vec3 getPosition();

    glm::mat4 getTransformation();

    GLfloat getAge();

    //return true if out of bounds for deletion
    GLboolean update(GLfloat timeStep, glm::vec3 domainSize);
};




#endif //A6_PROJECTILE_H
