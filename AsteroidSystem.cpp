//
// Created by evaughan on 12/10/20.
//

#include "AsteroidSystem.h"

GLfloat randNumber1( GLfloat max ) {
    return rand() / (GLfloat)RAND_MAX * max * 2.0 - max;
}

AsteroidSystem::AsteroidSystem() {

    for (int i = 0; i < 100; i ++) { // Make 100 asteroids
        Asteroid asteroid = Asteroid("textures/asteroid.jpg", // Texture path
                                     glm::vec3(randNumber1(20), randNumber1(20), randNumber1(20)), // Initial position
                                     glm::vec3(1, 1, 1),  // Axis of rotation
                                     glm::vec3(1, 1, 1)); // Scale

        asteroids.push_back(asteroid);
        translations.push_back(glm::vec3(randNumber1(2), randNumber1(2), randNumber1(2))); // Generate random initial translation vector
    }

}

void AsteroidSystem::draw(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    float angle = 0;

    float translationScalar = 0.01f;

    for (int i = 0; i < asteroids.size(); i++) {

        glm::vec3 translation = translations[i] * translationScalar;

        asteroids[i].draw(viewMatrix, projMatrix, translation, angle);
    }

}