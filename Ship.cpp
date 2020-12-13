//
// Created by ethan on 12/6/20.
//

#include "Ship.h"

Ship::Ship() {}

Ship::Ship(char* ShipTexturePath) {
    acceleration = 50.0f;
    velocity = glm::vec3(0);
    position = glm::vec3(0);

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

    shipShaderAttributes.vPos         = shipShader->getAttributeLocation("vPos");
    shipShaderAttributes.vNormal      = shipShader->getAttributeLocation("vNormal");
    shipShaderAttributes.vUV          = shipShader->getAttributeLocation("vUV");

    engineShader = new CSCI441::ShaderProgram( "shaders/engine.v.glsl", "shaders/engine.f.glsl" );
    engineShaderUniforms.mvpMatrix    = engineShader->getUniformLocation("mvpMatrix");
    engineShaderUniforms.time         = engineShader->getUniformLocation("time");
    engineShaderUniforms.forwardInput = engineShader->getUniformLocation("forwardInput");
    engineShaderAttributes.vPos       = engineShader->getAttributeLocation("vPos");

    projectileShader = new CSCI441::ShaderProgram( "shaders/projectile.v.glsl", "shaders/projectile.f.glsl" );
    projectileShaderUniforms.mvpMatrix= projectileShader->getUniformLocation("mvpMatrix");
    projectileShaderAttributes.vPos   = projectileShader->getAttributeLocation("vPos");

    model = new CSCI441::ModelLoader();
    model->loadModelFile("data/ship.obj");

    engineModel = new CSCI441::ModelLoader();
    engineModel->loadModelFile("data/engine.obj");

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
    lightColors[0] = glm::vec3(0.0f, 2.0f, 0.0f);
    glUniform3fv(shipShaderUniforms.pointLights[0].color, 1, &lightColors[0][0]);

    lightColors[1] = glm::vec3(0.0f, 0.0f, 4.0f);
    glUniform3fv(shipShaderUniforms.pointLights[1].color, 1, &lightColors[1][0]);

    lightColors[2] = glm::vec3(3600.0f, 3600.0f, 3000.0f);

    rotationSpeed = 2.0f;



    engineTransforms[0] = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-0.2f, -0.3f, 0.9f)), glm::vec3(0.35f, 0.75, 0.75));
    engineTransforms[1] = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-0.2f, -0.3f, -0.9f)), glm::vec3(0.35f, 0.75, 0.75));

    // Send directional light info
    directionalLight = glm::vec3(-1.0f, -1.0f, 0.0f);
    lightColors[3] = glm::vec3(0.0f, 50.0f, 0.0f);

}

void Ship::update(GLfloat yInput, GLfloat xInput, GLfloat flyInput, GLfloat shootInput, GLfloat timeStep, glm::vec3 shotDomainSize) {
    yaw += yInput * timeStep * rotationSpeed;
    pitch -= xInput * timeStep * rotationSpeed;

    yaw = glm::clamp(yaw, -1.57f, 1.57f);

    velocity *= max(0.5f, 1.0f - 0.07f * timeStep); // apply air resistance to make ship easier to control
    velocity += getForwardVector() * timeStep * flyInput * acceleration;

    engineShader->useProgram();
    glUniform1f(engineShaderUniforms.forwardInput, flyInput);


    if(shootInput > 0 && shotCooldown <= 0.0f ) {
        shots.push_back(Projectile(position + getForwardVector(), 40.0f * getForwardVector()));
        shotCooldown = 0.15f;
    }
    if(shotCooldown >= 0.0f) shotCooldown -= timeStep;

    for (int i = 0; i < shots.size(); i ++ ) {
        //shots update multiple times to avoid misses from overshooting
        for(int j = 1; j < 4; j ++ ) {
            if (shots[i].update(timeStep, shotDomainSize)) {
                shots.erase(shots.begin() + i);
                i--;
                j = 1000;
            }
        }
    }
}

void Ship::draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projectionMtx, GLfloat time) {
    shipShader->useProgram();

    modelMtx = glm::rotate(glm::rotate(modelMtx, pitch, glm::vec3(0, 1, 0)), yaw, glm::vec3(0, 0, 1));
    glm::mat4 mvpMatrix = projectionMtx * viewMtx * modelMtx;

    glUniformMatrix4fv(shipShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(shipShaderUniforms.modelMatrix, 1, GL_FALSE, &modelMtx[0][0]);
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glUniformMatrix3fv(shipShaderUniforms.normalMatrix, 1, GL_FALSE, &normalMtx[0][0]);

    // Send position of spot lights relative to ship position
    lightPositions[0] = glm::vec3(modelMtx * glm::vec4(-1.5f, -0.25f, -1.5f, 1.0f));
    glUniform3fv(shipShaderUniforms.pointLights[0].position, 1, &lightPositions[0][0]);
    lightPositions[1] = glm::vec3(modelMtx * glm::vec4(-1.5, -0.25f, 1.5f, 1.0f));
    glUniform3fv(shipShaderUniforms.pointLights[1].position, 1, &lightPositions[1][0]);

    lightPositions[2] = glm::vec3(modelMtx * glm::vec4(3, 0, 0, 1.0f));
    directionalLight = glm::vec3(modelMtx * glm::vec4(1, 0, 0, 0));

    glBindTexture(GL_TEXTURE_2D, shipTextureHandle);
    model->draw(shipShaderAttributes.vPos, shipShaderAttributes.vNormal, shipShaderAttributes.vUV, -1, -1, -1, -1,
                shipTextureHandle);

    engineShader->useProgram();
    glUniform1f(engineShaderUniforms.time, time);
    for (glm::mat4 engineTransform : engineTransforms) {

        mvpMatrix = projectionMtx * viewMtx * modelMtx * engineTransform;
        glUniformMatrix4fv(engineShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMatrix[0][0]);
        engineModel->draw(engineShaderAttributes.vPos, -1, -1, -1, -1, -1,
                          -1,
                          -1);
    }

    projectileShader->useProgram();
    for (Projectile & shot : shots) {
        mvpMatrix = projectionMtx * viewMtx * shot.getTransformation();
        glUniformMatrix4fv(projectileShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMatrix[0][0]);
        engineModel->draw(projectileShaderAttributes.vPos, -1, -1, -1, -1, -1,
                          -1,
                          -1);
    }
}

void Ship::sendLightingData(GLint pointLight1Location, GLint pointLight1Color, GLint pointLight2Location, GLint pointLight2Color, GLint directionalLightPosition, GLint directionalLightDirection, GLint directionalLightColor ){
    glUniform3fv(pointLight1Location, 1, &lightPositions[0][0]);
    glUniform3fv(pointLight1Color, 1, &lightColors[0][0]);

    glUniform3fv(pointLight2Location, 1, &lightPositions[1][0]);
    glUniform3fv(pointLight2Color, 1, &lightColors[1][0]);

    glUniform3fv(directionalLightPosition, 1, &lightPositions[2][0]);
    glUniform3fv(directionalLightColor, 1, &lightColors[2][0]);
    glUniform3fv(directionalLightDirection, 1, &directionalLight[0]);
}


glm::vec3 Ship::getPosition() {
    return position;
}

glm::vec3 Ship::getVelocity() {
    return velocity; //velocity is used to move asteroids around the ship for easier looping
}

glm::vec3 Ship::getForwardVector(){
    return glm::vec3(Ship::getTransform() * glm::vec4(1, 0, 0, 0));
}

glm::mat4 Ship::getTransform(){
    return glm::rotate(glm::rotate(glm::translate(glm::mat4(1), position), pitch, glm::vec3(0, 1, 0)), yaw, glm::vec3(0, 0, 1));
}

void Ship::sendViewDirection(glm::vec3 &camDir){
    glUniform3fv(shipShaderUniforms.viewDir, 1, &camDir[0]);
}

