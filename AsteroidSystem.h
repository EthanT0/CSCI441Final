//
// Created by evaughan on 12/10/20.
//

#ifndef CSCI441FINAL_ASTEROIDSYSTEM_H
#define CSCI441FINAL_ASTEROIDSYSTEM_H

#include "Asteroid.h"
#include "vector"

class AsteroidSystem {
    struct AsteroidShaderProgramUniforms {
        GLint mvpMatrix;                    // the MVP Matrix to apply
        GLint textureMap;

    };
    struct AsteroidShaderProgramAttributes {
        GLint vPos;                         // position of our vertex
        GLint tPosition;

    };
    GLuint asteroidTextureHandle;
    CSCI441::ShaderProgram* asteroidShaderProgram = nullptr;
    const char* textureFile;
    GLuint asteroidVAO, asteroidVBOs[2];

    struct AsteroidShaderProgramUniforms asteroidShaderProgramUniforms;
    struct AsteroidShaderProgramAttributes asteroidShaderProgramAttributes;


public:
    std::vector<Asteroid> asteroids;
    std::vector<glm::vec3> translations;

    AsteroidSystem();
    AsteroidSystem(char* texturePath);

    void draw(glm::mat4, glm::mat4);
    void sendMvpMatrix();
};


#endif //CSCI441FINAL_ASTEROIDSYSTEM_H
