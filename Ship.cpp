//
// Created by ethan on 12/6/20.
//

#include "Ship.h"

Ship::Ship() {}

Ship::Ship(char* ShipTexturePath) {
    //position = glm::vec3(0);

    shipShader = new CSCI441::ShaderProgram( "shaders/ship.v.glsl", "shaders/ship.f.glsl" );
    shipShaderUniforms.mvpMatrix      = shipShader->getUniformLocation("mvpMatrix");
    shipShaderUniforms.shipTex = shipShader->getUniformLocation("shipTex");

    shipShaderAttributes.vPos         = shipShader->getAttributeLocation("vPos");
    shipShaderAttributes.vUV         = shipShader->getAttributeLocation("vUV");


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

    glBindTexture(GL_TEXTURE_2D, shipTextureHandle);

    //model->draw(positionLocation, normalLocation, texCoordLocation, matDiffLocation, matSpecLocation, matShinLocation, matAmbLocation, diffuseTexture)
    model->draw( shipShaderAttributes.vPos, shipShaderAttributes.vNormal,shipShaderAttributes.vUV, -1, -1, -1, -1, shipTextureHandle );      // TODO #18 set actual vertex texture coordinate location
}

glm::vec3 Ship::getPosition() {
    return glm::vec3(0);
}