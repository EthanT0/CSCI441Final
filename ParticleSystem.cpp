//
// Created by ethan on 11/23/20.
//

#include "ParticleSystem.h"

GLfloat randNumber( GLfloat max ) {
    return rand() / (GLfloat)RAND_MAX * max * 2.0 - max;
}

ParticleSystem::ParticleSystem(){}

ParticleSystem::ParticleSystem(glm::vec3 originPoint, GLint maxParticles, GLint vPosAttribute, GLint texHandle, GLuint projMatrixUniform, GLuint mvMatrixUniform): origin(originPoint), maxParticles(maxParticles), texHandle(texHandle), projMatrixUniform(projMatrixUniform), mvMatrixUniform(mvMatrixUniform){


    for( int i = 0; i < maxParticles; i++ ) {
        glm::vec3 pos = originPoint;

        float randomSign = randNumber(1);

        if (randomSign < 0.5) {
            randomSign = -1.0f;
        } else {
            randomSign = 1.0f;
        }

        Particle p = Particle(pos, glm::vec3(randNumber(40),randNumber(40) * randomSign,randNumber(40)));

        particles.push_back(p);
        updatedPositions.push_back(p.getPosition());
        indices.push_back(i);
    }
    glGenVertexArrays( 1, &pvao );
    glBindVertexArray( pvao );

    glGenBuffers( 1, &pvbo );

    glGenBuffers( 1, &pibo );


    glBindBuffer(GL_ARRAY_BUFFER, pvbo);

    glBufferData( GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW );

    glEnableVertexAttribArray( vPosAttribute );
    glVertexAttribPointer( vPosAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*) 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,  maxParticles * sizeof(GLshort), &indices[0], GL_DYNAMIC_DRAW );
}

void ParticleSystem::draw(glm::mat4 viewMat, glm::mat4 projMat, glm::vec3 cameraPos, glm::vec3 lookingVector){

    //model matrix is origin offset
    glm::mat4 modelMat = glm::translate(glm::mat4(1), origin);

    glm::mat4 mvMat = viewMat * modelMat;

    glBindVertexArray( pvao );
    glBindTexture(GL_TEXTURE_2D, texHandle);


    std::vector< std::pair <GLfloat,GLushort>> iboDistances;

    //sort pairs of IBO and distances by distance
    for (int i=0; i<maxParticles; i++) iboDistances.push_back( std::make_pair(particles[i].distanceToCamera(modelMat, cameraPos, lookingVector), i) );
    sort(iboDistances.rbegin(), iboDistances.rend());

    //create a new vector of the IBOs
    std::vector<GLushort> ibos;
    for (std::pair <GLfloat,GLint> element : iboDistances) ibos.push_back(element.second);

    glBindBuffer( GL_ARRAY_BUFFER, pvbo );
    glBufferData( GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW );


    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pibo );
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * maxParticles, &ibos[0]);

    glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, &mvMat[0][0]);
    glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, &projMat[0][0]);

    glDrawElements( GL_POINTS, maxParticles, GL_UNSIGNED_SHORT, (void*)0 );


}

void ParticleSystem::update(GLfloat timeStep, glm::vec3 wrapPosition, glm::vec3 wrapSize){
    for(int i = 0; i < maxParticles; i ++ ) {
        updatedPositions[i] = particles[i].update(timeStep, wrapPosition, wrapSize);
    }
}

void ParticleSystem::reset() {
    for(int i = 0; i < maxParticles; i ++ ) {
        particles[i].resetPosition();
    }
}