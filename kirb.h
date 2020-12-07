//
// Created by ethan on 10/13/20.
//

#ifndef MIDTERM_KIRB_H
#define MIDTERM_KIRB_H

#include <CSCI441/objects.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class kirb {
    GLint mvpMatrixUniform;
    GLint normalMatrixUniform;
    GLint objectMatrixUniform;
    GLint materialColorUniform;

    //storing mvpMatrix in kirb to make passing data to shaders easier
    glm::mat4 mvpMatrix;

    glm::mat4 bodyTransform;
    glm::mat4 leftCheekTransform;
    glm::mat4 rightCheekTransform;
    glm::mat4 leftArmTransform;
    glm::mat4 rightArmTransform;
    glm::mat4 leftShoeTransform;
    glm::mat4 rightShoeTransform;

    glm::mat4 propellorRodTransform;
    glm::mat4 propellorBottomTransform;
    glm::mat4 propellorTopTransform;

    GLfloat animationTime;

    GLfloat kirbAnimationDelta = 3.0f;
    GLfloat kirbAnimationBase = 3.0f;
    GLfloat kirbAcceleration = 30.0f;
    GLfloat rotationSpeed = 5.0f;

    GLfloat kirbCopterAngle = 0.0f;
    glm::vec3 kirbCopterVelocity = glm::vec3(0, 0, 0);
    glm::vec3 kirbCopterPosition = glm::vec3(0, 0, 0);

    GLfloat gravity = 0.06f;
    GLfloat airResistance = 1;
    GLfloat hoverHeight = 4.0f;

    void drawKirbSphere(glm::mat4 localTransform, glm::vec3 color);
    void drawKirbCylinder(glm::mat4 localTransform, glm::vec3 color);
    void drawKirbCube(glm::mat4 localTransform, glm::vec3 color);

public:
    kirb();
    kirb(glm::vec3 startingPos, GLfloat startingAngle, GLint mvpMatrix, GLint normalMatrix, GLint objectMatrix, GLint materialColor); //pass uniform locations to simplify later function calls

    void move(GLfloat verticalInput, GLfloat horizontalInput, GLfloat fly = 0.0f, GLfloat deltaTime = 0.004f);
    void animate(GLfloat timeStep);
    void draw(glm::mat4 viewMtx, glm::mat4 projMtx, glm::mat4 kirbLocalTransform = glm::mat4(1));

    glm::vec3 getPosition();

    glm::vec3 getForwardVector();

    glm::mat4 getTransformMatrix();

    void heightmapCollision(GLfloat height);

    void boundaryCollision(glm::vec3 min, glm::vec3 max);
};


#endif //MIDTERM_KIRB_H