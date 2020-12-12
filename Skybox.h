//
// Created by ethan on 12/6/20.
//

#ifndef FINALPROJ_SKYBOX_H
#define FINALPROJ_SKYBOX_H


#include <CSCI441/ShaderUtils.hpp>
#include <CSCI441/objects.hpp>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <CSCI441/ShaderProgram.hpp>

struct skyboxShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint skybox;                 // Skybox texture sampler
};

struct skyboxShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
};

class Skybox {
    GLuint skyboxVAO, skyboxVBO, skyboxIBO;
    GLuint skyboxTextureHandle;

    CSCI441::ShaderProgram *skyboxShader = nullptr;   // the wrapper for our shader program

    struct skyboxShaderUniforms skyboxShaderUniforms;
    struct skyboxShaderAttributes skyboxShaderAttributes;

public:
    Skybox();

    Skybox(char** skyboxPathways);

    void draw(glm::mat4 rotationMtx);

};


#endif //FINALPROJ_SKYBOX_H
