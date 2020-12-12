
#include "Asteroid.h"
Asteroid::Asteroid() {}
Asteroid::Asteroid(glm::vec3 initialPosition, glm::vec3 rotAxis, glm::vec3 scale) {
    translationVector = initialPosition;
    rotationAngle = 0;
    rotationAxis = rotAxis;
    scaleVector = scale;


}

glm::mat4 Asteroid::draw(glm::mat4 viewMtx, glm::mat4 projMtx, glm::vec3 translation, float rotAngle) {
    translationVector = translation;
    rotationAngle = rotAngle;

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translationVector);
    modelMatrix = glm::rotate( modelMatrix, rotationAngle, rotationAxis );
    modelMatrix = glm::scale(modelMatrix, scaleVector);
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMatrix;
    return mvpMtx;
}

glm::vec3 Asteroid::getPosition() {

    return translationVector;

}