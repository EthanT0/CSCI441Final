//
// Created by ethan on 12/6/20.
//

#include "Ship.h"

Ship::Ship() {}

Ship::Ship(char* ShipTexturePath) {
    //position = glm::vec3(0);
    shipShader = new CSCI441::ShaderProgram( "shaders/ship.v.glsl", "shaders/ship.f.glsl" );
    shipShaderUniforms.mvpMatrix      = shipShader->getUniformLocation("mvpMatrix");
    shipShaderUniforms.modelMatrix    = shipShader->getUniformLocation("modelMatrix");
    shipShaderUniforms.normalMatrix   = shipShader->getUniformLocation("normalMatrix");
    shipShaderUniforms.viewDir        = shipShader->getUniformLocation("viewDir");
    shipShaderUniforms.shipTex        = shipShader->getUniformLocation("shipTex");
    // Point lights uniforms
    shipShaderUniforms.pointLights[0].position     = shipShader->getUniformLocation("pointLights[0].position");
    shipShaderUniforms.pointLights[0].color        = shipShader->getUniformLocation("pointLights[0].color");
    shipShaderUniforms.pointLights[1].position     = shipShader->getUniformLocation("pointLights[1].position");
    shipShaderUniforms.pointLights[1].color        = shipShader->getUniformLocation("pointLights[1].color");
    // Directional lights uniforms
    shipShaderUniforms.directionalLights[0].direction     = shipShader->getUniformLocation("directionalLights[0].direction");
    shipShaderUniforms.directionalLights[0].color         = shipShader->getUniformLocation("directionalLights[0].color");

    shipShaderAttributes.vPos         = shipShader->getAttributeLocation("vPos");
    shipShaderAttributes.vNormal      = shipShader->getAttributeLocation("vNormal");
    shipShaderAttributes.vUV          = shipShader->getAttributeLocation("vUV");


    model = new CSCI441::ModelLoader();
    model->loadModelFile("data/ship.obj");

    glGenTextures(1, &shipTextureHandle);
    glBindTexture(GL_TEXTURE_2D, shipTextureHandle);
    int width, height, channels;
    unsigned char *data = stbi_load( ShipTexturePath, &width, &height, &channels, 0);
    if(data) {
        fprintf(stdout, "Loaded texture [%s]\n", ShipTexturePath);
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
        );
    }
    else {
        fprintf(stdout, "Texture failed to load [%s]\n", ShipTexturePath );
    }
    stbi_image_free(data);

    // Send point light values
    shipShader->useProgram();

    // Send spot light colors
    glm:: vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
    glUniform3fv(shipShaderUniforms.pointLights[0].color, 1, &color[0]);
    color = glm::vec3(0.0f, 1.0f, 0.0f);
    glUniform3fv(shipShaderUniforms.pointLights[1].color, 1, &color[0]);

    // Send directional light info
    glm::vec3 direction = glm::vec3(-1.0f, -1.0f, 0.0f);
    color = glm::vec3(0.8f, 0.8f, 0.8f);
    glUniform3fv(shipShaderUniforms.directionalLights[0].direction, 1, &direction[0]);
    glUniform3fv(shipShaderUniforms.directionalLights[0].color, 1, &color[0]);

}

void Ship::rotate(GLfloat xInput, GLfloat yInput, GLfloat timeStep) {
    yaw += xInput * timeStep;
    pitch += yInput * timeStep;
}

void Ship::rotatex(GLfloat xInput, GLfloat timeStep) {
    pitch = xInput * timeStep;
}

void Ship::rotatey(GLfloat yInput, GLfloat timeStep) {
    yaw = yInput * timeStep;
}

void Ship::draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 rotationMtx){
    shipShader->useProgram();

    modelMtx = glm::rotate( glm::rotate(modelMtx, pitch, glm::vec3(0, 1, 0)), yaw, glm::vec3(0, 0, 1) );
    glm::mat4 mvpMatrix = rotationMtx * viewMtx * modelMtx;

    glUniformMatrix4fv(shipShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMatrix[0][0] );
    glUniformMatrix4fv(shipShaderUniforms.modelMatrix, 1, GL_FALSE, &modelMtx[0][0] );
    glm::mat3 normalMtx = glm::mat3( glm::transpose(glm::inverse(modelMtx)));
    glUniformMatrix3fv(shipShaderUniforms.normalMatrix, 1, GL_FALSE, &normalMtx[0][0] );

    // Send position of spot lights relative to ship position
    glm::vec4 position = modelMtx * glm::vec4(-1.5f, -0.25f, -1.5f, 1.0f);
    glUniform3fv(shipShaderUniforms.pointLights[0].position, 1, &position[0]);
    position = modelMtx * glm::vec4(-1.5f, -0.25f, 1.5f, 1.0f);
    glUniform3fv(shipShaderUniforms.pointLights[1].position, 1, &position[0]);

    glBindTexture(GL_TEXTURE_2D, shipTextureHandle);

    //model->draw(positionLocation, normalLocation, texCoordLocation, matDiffLocation, matSpecLocation, matShinLocation, matAmbLocation, diffuseTexture)
    model->draw( shipShaderAttributes.vPos, shipShaderAttributes.vNormal,shipShaderAttributes.vUV, -1, -1, -1, -1, shipTextureHandle );      // TODO #18 set actual vertex texture coordinate location
}

glm::vec3 Ship::getPosition() {
    return glm::vec3(0);
}

void Ship::sendViewDirection(glm::vec3 &camDir){
    glUniform3fv(shipShaderUniforms.viewDir, 1, &camDir[0]);
}