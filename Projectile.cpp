//
// Created by ethan on 12/12/20.
//

#include "Projectile.h"

Projectile::Projectile(glm::vec3 initialPosition, glm::vec3 velocity) : position(initialPosition), velocity(velocity){
    velocity = glm::normalize(velocity);
    glm::vec3 rotateAround = glm::cross(glm::vec3(1, 0, 0), velocity );
    GLfloat angle = acos(glm::dot(velocity, glm::vec3(1, 0, 0)));

    rotationTransform = glm::rotate(glm::mat4(1), angle, rotateAround);
}


glm::vec3 Projectile::getVelocity(){
    return velocity;
}

glm::vec3 Projectile::getPosition(){
    return position;
}

glm::mat4 Projectile::getTransformation(){
    return glm::translate(glm::mat4(1), position) * rotationTransform;
}

GLfloat Projectile::getAge(){
    return age;
}

GLboolean Projectile::update(GLfloat timeStep, glm::vec3 domainSize){
    position += velocity * timeStep;
    glm::vec3 posCubeTest = glm::abs(position);

    if(posCubeTest.x > domainSize.x / 2 || posCubeTest.y > domainSize.y / 2 || posCubeTest.z > domainSize.z / 2 ) return true;
    return false;
}