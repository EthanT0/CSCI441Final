
#include "Asteroid.h"
Asteroid::Asteroid() {}
Asteroid::Asteroid(glm::vec3 initialPosition, glm::vec3 velocity, glm::vec3 rotAxis, GLfloat scale, GLfloat rotationSpeed) : scale(scale), rotationSpeed(rotationSpeed), velocity(velocity), translationVector(initialPosition), rotationAngle(0), rotationAxis(rotAxis) {
    scaleVector = glm::vec3(scale);
}

glm::mat4 Asteroid::getTransformation() {
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translationVector);

    modelMatrix = glm::rotate( modelMatrix, rotationAngle, rotationAxis );
    modelMatrix = glm::scale(modelMatrix, scaleVector);
    return modelMatrix;
}

glm::vec3 Asteroid::getPosition() {

    return translationVector;

}

void Asteroid::update(GLfloat timeStep, glm::vec3 shipVelocity, glm::vec3 domainSize) {
    translationVector += (velocity - shipVelocity) * timeStep;
    rotationAngle += rotationSpeed * timeStep;

    glm::vec3 domainPosition = translationVector + domainSize * 0.5f;
    domainPosition = glm::mod(domainPosition, domainSize);
    translationVector = domainPosition - domainSize * 0.5f;
}

GLboolean Asteroid::collisionTest(glm::vec3 point, GLfloat pointRadius) {
    glm::vec3 delta = translationVector - point;
    return dot(delta, delta) < (scale + pointRadius) * (scale + pointRadius);
}

void Asteroid::bounce(glm::vec3 normal, GLfloat bounciness) {
       normal = glm::normalize(normal);
       velocity -= (1 + bounciness) * normal * glm::dot(normal, velocity);
}

glm::vec3 Asteroid::getVelocity() {
    return velocity;
}
