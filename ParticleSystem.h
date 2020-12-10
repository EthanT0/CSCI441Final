//
// Created by ethan on 11/23/20.
//

#ifndef A5_PARTICLESYSTEM_H
#define A5_PARTICLESYSTEM_H

#include <CSCI441/ShaderUtils.hpp>
#include <CSCI441/objects.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

#include <vector>
#include<bits/stdc++.h>

#include "Particle.h"

#include <iostream>


class ParticleSystem {
    GLint maxParticles;
    GLuint projMatrixUniform, mvMatrixUniform;
    GLuint pvbo, pvao, pibo;
    GLuint texHandle;



    glm::vec3 origin;
    std::vector<Particle> particles;
    std::vector<GLushort> indices;
    std::vector<glm::vec3> updatedPositions;

public:
    ParticleSystem();

    ParticleSystem(glm::vec3 originPoint, GLint maxParticles, GLint vPos, GLint texHandle, GLuint projMatrixUniform, GLuint viewMatrixUniform);

    void draw(glm::mat4 viewMat, glm::mat4 projMat, glm::vec3 cameraPos, glm::vec3 lookingVector);

    void update(GLfloat timeStep, glm::vec3 wrapPosition, glm::vec3 wrapSize);
    void reset();

};


#endif //A5_PARTICLESYSTEM_H
