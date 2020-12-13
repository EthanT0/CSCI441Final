//
// Created by evaughan on 12/10/20.
//

#ifndef CSCI441FINAL_ASTEROIDSYSTEM_H
#define CSCI441FINAL_ASTEROIDSYSTEM_H

#include "Asteroid.h"
#include "vector"
#include "Ship.h"

class AsteroidSystem {
    struct AsteroidShaderProgramUniforms {
        GLint mvpMatrix;                    // the MVP Matrix to apply
        GLint modelMatrix;
        GLint normalMatrix;
        GLint asteroidTex;
        GLint viewDir;
        shipPointLight pointLights[2];
        shipDirectionalLight directionalLights[3];

    };
    struct AsteroidShaderProgramAttributes {
        GLint vPos;                         // position of our vertex
        GLint vUV;
        GLint vNormal;
    };
    GLuint asteroidTextureHandle;
    CSCI441::ShaderProgram* asteroidShaderProgram = nullptr;
    const char* textureFile;
    GLuint asteroidVAO, asteroidVBO, asteroidIBO;
    GLint asteroidVertexCount;

    struct AsteroidShaderProgramUniforms asteroidShaderProgramUniforms;
    struct AsteroidShaderProgramAttributes asteroidShaderProgramAttributes;

    glm::vec3 domainScale;

public:
    void setLightingParameters(Ship &ship, glm::vec3 &camDir);

    std::vector<Asteroid> asteroids;

    AsteroidSystem();

    AsteroidSystem(char* texturePath, glm::vec3 domainScale, GLint initialCount = 60);

    GLboolean update(GLfloat timeStep, Ship& ship);

    void draw(glm::mat4, glm::mat4);

    void spawnAsteroid();

    void sendMvpMatrix();
};

#endif //CSCI441FINAL_ASTEROIDSYSTEM_H
