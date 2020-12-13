//
// Created by ethan on 12/6/20.
//

#ifndef FINALPROJ_SHIP_H
#define FINALPROJ_SHIP_H

#include <vector>

#include <CSCI441/ShaderUtils.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/modelLoader.hpp>      // .obj files

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include "Projectile.h"

struct shipPointLight{              // stores point light information
    GLint position;
    GLint color;
};

struct shipDirectionalLight{
    GLint position;
    GLint direction;
    GLint color;
};

struct shipShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint modelMatrix;
    GLint normalMatrix;
    GLint viewDir;
    GLint shipTex;                 // Skybox texture sampler
    shipPointLight pointLights[2];
    shipDirectionalLight directionalLights[3];
};

struct shipShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
    GLint vNormal;
    GLint vUV;
};

struct engineShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint time;
    GLint forwardInput;
};

struct engineShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
};

struct projectileShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
};

struct projectileShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
};

class Ship {
    GLfloat acceleration;

    glm::vec3 position;
    glm::vec3 velocity;

    glm::vec3 lightPositions[3];
    glm::vec3 lightColors[3];
    glm::vec3 directionalLight;

    GLfloat rotationSpeed;
    GLfloat pitch, yaw;

    glm::mat4 engineTransforms[2];

    GLuint shipTextureHandle;

    CSCI441::ModelLoader* model = nullptr;
    CSCI441::ModelLoader* engineModel = nullptr;

    CSCI441::ShaderProgram *shipShader = nullptr;   // the wrapper for our shader program
    CSCI441::ShaderProgram *engineShader = nullptr;   // the wrapper for our shader program
    CSCI441::ShaderProgram *projectileShader = nullptr;   // the wrapper for our shader program


    struct shipShaderUniforms shipShaderUniforms;
    struct shipShaderAttributes shipShaderAttributes;

    struct engineShaderUniforms engineShaderUniforms;
    struct engineShaderAttributes engineShaderAttributes;

    struct projectileShaderUniforms projectileShaderUniforms;
    struct projectileShaderAttributes projectileShaderAttributes;

    GLfloat shotCooldown = 0;
public:
    std::vector<Projectile> shots;

    Ship();

    Ship(char* ShipTexturePath);

    void update(GLfloat xInput, GLfloat yInput, GLfloat flyInput, GLfloat shootInput, GLfloat timeStep, glm::vec3 shotDomainSize);

    void rotatex(GLfloat xInput, GLfloat timeStep);

    void rotatey(GLfloat yInput, GLfloat timeStep);

    void draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 rotationMtx, GLfloat time = 0.0f);

    void sendLightingData(GLint pointLight1Location, GLint pointLight1Color, GLint pointLight2Location, GLint pointLight2Color, GLint directionalLightPosition, GLint directionalLightDirection, GLint directionalLightColor );

    glm::vec3 getPosition();

    glm::vec3 getVelocity();

    void sendViewDirection(glm::vec3 &camDir);

    glm::vec3 getForwardVector();

    glm::mat4 getTransform();

    glm::vec3 shipviewCameraVector();
};


#endif //FINALPROJ_SHIP_H
