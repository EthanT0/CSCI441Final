//
// Created by evaughan on 12/10/20.
//

#ifndef CSCI441FINAL_ASTEROIDSYSTEM_H
#define CSCI441FINAL_ASTEROIDSYSTEM_H

#include "Asteroid.h"
#include "vector"

class AsteroidSystem {


public:
    std::vector<Asteroid> asteroids;
    std::vector<glm::vec3> translations;

    AsteroidSystem();

    void draw(glm::mat4, glm::mat4);
};


#endif //CSCI441FINAL_ASTEROIDSYSTEM_H
