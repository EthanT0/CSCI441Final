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

struct shipPointLight{              // stores point light information
    GLint position;
    GLint color;
};

struct shipDirectionalLight{
    GLint direction;
    GLint color;
};

struct shipSpotLight{
    GLint position;
    GLint direction;
    GLint color;
    GLint cutOff;
    GLint outerCutOff;
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

class Ship {

    GLfloat pitch, yaw;
    //glm::vec3 position;

    GLuint shipTextureHandle;

    CSCI441::ModelLoader* model = nullptr;

    CSCI441::ShaderProgram *shipShader = nullptr;   // the wrapper for our shader program

    struct shipShaderUniforms shipShaderUniforms;
    struct shipShaderAttributes shipShaderAttributes;

public:
    Ship();

    Ship(char* ShipTexturePath);

    void rotate(GLfloat xInput, GLfloat yInput, GLfloat timeStep);

    void rotatex(GLfloat xInput, GLfloat timeStep);

    void rotatey(GLfloat yInput, GLfloat timeStep);

    void draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 rotationMtx);

    glm::vec3 getPosition();

    void sendViewDirection(glm::vec3 &camDir);
};


#endif //FINALPROJ_SHIP_H