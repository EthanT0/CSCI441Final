//
// Created by ethan on 10/13/20.
//

#include "kirb.h"
#include <iostream>

kirb::kirb(glm::vec3 startingPos, GLfloat startingAngle, GLint mvpMatrix, GLint normalMatrix, GLint objectMatrix, GLint materialColor) : kirbCopterPosition(startingPos), kirbCopterAngle(startingAngle), mvpMatrixUniform(mvpMatrix), normalMatrixUniform(normalMatrix), objectMatrixUniform(objectMatrix), materialColorUniform(materialColor), animationTime(0) {
    kirbCopterVelocity = glm::vec3(0);
    animate(0);
}

kirb::kirb() : mvpMatrixUniform(0), normalMatrixUniform(0), materialColorUniform(0) {}

void kirb::animate(GLfloat timeStep){
    kirbCopterPosition += kirbCopterVelocity * timeStep;

    //apply air resistance
    kirbCopterVelocity *= (1 - airResistance * timeStep);

    //interpolate between current animation speed and base animation speed
    kirbAnimationDelta =  kirbAnimationBase * (6 * timeStep) + (1.0f - 6 * timeStep) * (kirbAnimationDelta) + 4 * length(kirbCopterVelocity) * kirbAnimationBase * timeStep;
    timeStep *= kirbAnimationDelta;

    animationTime += timeStep;

    bodyTransform = glm::rotate(glm::translate(glm::mat4(1), kirbCopterPosition), 1.57f -kirbCopterAngle, glm::vec3(0, 1, 0));
    leftCheekTransform = bodyTransform * glm::scale(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0.45, -0.1, 0.88)), 0.7f, glm::vec3(0, 1, 0)),  glm::vec3(0.22, 0.07, 0.03));
    rightCheekTransform = bodyTransform * glm::scale(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-0.45, -0.1, 0.88)), -0.7f, glm::vec3(0, 1, 0)),  glm::vec3(0.22, 0.07, 0.03));

    GLfloat armPivotHeight = 0;
    GLfloat armAngle = (sin(animationTime) - 1.0f) * 0.3f - 0.3f;
    leftArmTransform = bodyTransform * glm::scale(glm::translate(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, armPivotHeight, 0)), armAngle, glm::vec3(0.435889894354, 0, 0.9)), glm::vec3(0.4, 0.8f - armPivotHeight, -0.2)), glm::vec3(0.3, 0.5, 0.3));
    rightArmTransform = bodyTransform * glm::scale(glm::translate(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, armPivotHeight, 0)), -armAngle, glm::vec3(-0.435889894354, 0, 0.9)), glm::vec3(-0.4, 0.8f - armPivotHeight, -0.2)), glm::vec3(0.3, 0.5, 0.3));

    GLfloat shoeAngle = sin(animationTime) * 0.25 + 0.25;
    leftShoeTransform = bodyTransform * glm::scale(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-0.4, -0.8, -0.8)), shoeAngle, glm::vec3(1, 0, 0)),  glm::vec3(0.4, 0.5, 0.3));
    rightShoeTransform = bodyTransform * glm::scale(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0.4, -0.8, -0.8)), shoeAngle, glm::vec3(1, 0, 0)),  glm::vec3(0.4, 0.5, 0.3));

    propellorRodTransform = bodyTransform * glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 0.9, 0)),  glm::vec3(0.1f, 1.5f, 0.1f));
    propellorBottomTransform = bodyTransform * glm::scale(glm::translate(glm::rotate(glm::mat4(1), animationTime, glm::vec3(0, 1, 0)), glm::vec3(0, 1.5, 0)), glm::vec3(3, 0.05, 0.3));
    propellorTopTransform = bodyTransform * glm::scale(glm::translate(glm::rotate(glm::mat4(1), -animationTime * 1.01f, glm::vec3(0, 1, 0)), glm::vec3(0, 2.0f, 0)), glm::vec3(3, 0.05, 0.3));
}

void kirb::drawKirbSphere(glm::mat4 worldTransform, glm::vec3 color) {
    glm::mat4 mvpPartMtx = mvpMatrix * worldTransform;

    glm::mat4 normalMtx = glm::mat3(glm::transpose(	glm::inverse(worldTransform)));
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, &mvpPartMtx[0][0] );
    glUniformMatrix4fv(normalMatrixUniform, 1, GL_FALSE, &normalMtx[0][0]);
    glUniformMatrix4fv(objectMatrixUniform, 1, GL_FALSE, &worldTransform[0][0]);

    glUniform3fv(materialColorUniform, 1, &color[0]);
    CSCI441::drawSolidSphere( 1.0f, 16, 16 );
}

void kirb::drawKirbCylinder(glm::mat4 worldTransform, glm::vec3 color){
    glm::mat4 mvpPartMtx = mvpMatrix * worldTransform;

    glm::mat4 normalMtx = glm::mat3(glm::transpose(	glm::inverse(worldTransform)));
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, &mvpPartMtx[0][0] );
    glUniformMatrix4fv(normalMatrixUniform, 1, GL_FALSE, &normalMtx[0][0]);
    glUniformMatrix4fv(objectMatrixUniform, 1, GL_FALSE, &worldTransform[0][0]);

    glUniform3fv(materialColorUniform, 1, &color[0]);
    CSCI441::drawSolidCylinder( 1.0f, 1.0f, 1, 2, 16 );
}

void kirb::drawKirbCube(glm::mat4 worldTransform, glm::vec3 color){
    glm::mat4 mvpPartMtx = mvpMatrix * worldTransform;

    glm::mat4 normalMtx = glm::mat3(glm::transpose(	glm::inverse(worldTransform)));
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, &mvpPartMtx[0][0] );
    glUniformMatrix4fv(normalMatrixUniform, 1, GL_FALSE, &normalMtx[0][0]);
    glUniformMatrix4fv(objectMatrixUniform, 1, GL_FALSE, &worldTransform[0][0]);

    glUniform3fv(materialColorUniform, 1, &color[0]);
    CSCI441::drawSolidCube( 1.0f );
}

void kirb::draw(glm::mat4 viewMtx, glm::mat4 projMtx, glm::mat4 kirbLocalTransform) {

    glm::vec3 kirbBodyColor = glm::vec3(0.95, 0.65, 0.75);
    glm::vec3 kirbCheekColor = glm::vec3(0.87, 0.44, 0.55);
    glm::vec3 kirbArmColor = glm::vec3(0.9, 0.6, 0.7);
    glm::vec3 kirbShoeColor = glm::vec3(0.8, 0.11, 0.25);
    glm::vec3 propellorColor = glm::vec3(0.25);
    //set mvpMatrix for kirb's world transform so local transformations end up in the right spots
    mvpMatrix = projMtx * viewMtx * kirbLocalTransform;


    drawKirbSphere(bodyTransform, kirbBodyColor);
    drawKirbSphere(leftCheekTransform, kirbCheekColor);
    drawKirbSphere(rightCheekTransform, kirbCheekColor);
    drawKirbSphere(leftShoeTransform, kirbShoeColor);
    drawKirbSphere(rightShoeTransform, kirbShoeColor);
    drawKirbSphere(leftArmTransform, kirbArmColor);
    drawKirbSphere(rightArmTransform, kirbArmColor);

    drawKirbCylinder(propellorRodTransform, propellorColor);
    drawKirbCube(propellorBottomTransform, propellorColor);
    drawKirbCube(propellorTopTransform, propellorColor);
}

void kirb::move(GLfloat vertical, GLfloat horizontal, GLfloat fly, GLfloat deltaTime){
    //move forward according to value of vertical input (W/S)
    kirbCopterVelocity += vertical * kirbAcceleration * deltaTime * glm::vec3(cos(kirbCopterAngle), 0, sin(kirbCopterAngle));
    //turn forward according to value of horizontal input (A/D)
    kirbCopterAngle += horizontal * rotationSpeed * deltaTime;
    kirbCopterVelocity.y += 2 * gravity * fly;

}

glm::vec3 kirb::getPosition(){
    return kirbCopterPosition;
}

glm::vec3 kirb::getForwardVector() {
    return glm::vec3(cos(kirbCopterAngle), 0, sin(kirbCopterAngle));
}

void kirb::heightmapCollision(GLfloat groundHeight) {
    //kirb's origin is 1 above his lowest point
    groundHeight += 1;

    //hit the ground
    if(groundHeight > kirbCopterPosition.y) {
        kirbCopterPosition.y = groundHeight;
        kirbCopterVelocity.y = 0;
    }

    if(kirbCopterPosition.y < groundHeight + hoverHeight) {
        kirbCopterVelocity.y += (groundHeight + hoverHeight - kirbCopterPosition.y) * gravity ;
    }

    else kirbCopterVelocity.y -= gravity;

}

void kirb::boundaryCollision(glm::vec3 boundaryMin, glm::vec3 boundaryMax) {
    //collisions with boundaries
    if(kirbCopterPosition.x < boundaryMin.x ) {
        kirbCopterVelocity.x = abs(kirbCopterVelocity.x);
    }
    if(kirbCopterPosition.z < boundaryMin.z ) {
        kirbCopterVelocity.z = abs(kirbCopterVelocity.z);
    }
    if(kirbCopterPosition.x > boundaryMax.x ) {
        kirbCopterVelocity.x = -abs(kirbCopterVelocity.x);
    }
    if(kirbCopterPosition.z > boundaryMax.z ) {
        kirbCopterVelocity.z = -abs(kirbCopterVelocity.z);
    }
    kirbCopterPosition.x = glm::clamp(kirbCopterPosition.x, boundaryMin.x, boundaryMax.x);
    kirbCopterPosition.y = glm::clamp(kirbCopterPosition.y, boundaryMin.y, boundaryMax.y);
    kirbCopterPosition.z = glm::clamp(kirbCopterPosition.z, boundaryMin.z, boundaryMax.z);
}

glm::mat4 kirb::getTransformMatrix(){
    return bodyTransform;
}