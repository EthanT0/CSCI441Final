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

// include C/C++ libraries
#include <cstdio>				        // for printf functionality
#include <cstdlib>				        // for exit functionality
#include <vector>                       // for vector functionality

// include our class CSCI441 libraries
#include <stb_image.h>
#include <CSCI441/OpenGLUtils.hpp>      // to print info about OpenGL
#include <CSCI441/objects.hpp>          // to render our 3D primitives
#include <CSCI441/ShaderProgram.hpp>    // a wrapper class for Shader Programs



class Asteroid {
    struct AsteroidShaderProgramUniforms {
        GLint mvpMatrix;                    // the MVP Matrix to apply
        GLint textureMap;

    };
    struct AsteroidShaderProgramAttributes {
        GLint vPos;                         // position of our vertex
        GLint tPosition;

    };

    const char* textureFile;
    GLuint asteroidTextureHandle;
    GLuint asteroidVAO, asteroidVBOs[2];

    struct AsteroidShaderProgramUniforms asteroidShaderProgramUniforms;
    struct AsteroidShaderProgramAttributes asteroidShaderProgramAttributes;
    CSCI441::ShaderProgram* asteroidShaderProgram = nullptr;


public:
    Asteroid();
    Asteroid(char* texturePath, glm::vec3 initialPosition, glm::vec3 rotAxis, glm::vec3 scale);
    float rotationAngle;
    glm::vec3 translationVector;
    glm::vec3 rotationAxis;
    glm::vec3 scaleVector;
    glm::vec3 getPosition();
    void draw(glm::mat4 viewMtx, glm::mat4 projMtx, glm::vec3 translation, float rotAngle);


};


#endif //FINAL_ASTEROID_H
