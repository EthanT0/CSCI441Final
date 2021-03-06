//
// Created by evaughan on 12/10/20.
//

#include "AsteroidSystem.h"

GLfloat randNumber1( GLfloat max ) {
    return rand() / (GLfloat)RAND_MAX * max * 2.0 - max;
}

AsteroidSystem::AsteroidSystem(){}

AsteroidSystem::AsteroidSystem(char* texturePath, glm::vec3 domainScale, GLint initialCount) {


    asteroidTextureHandle = 0;
    this->domainScale = domainScale;

    asteroidShaderProgram = new CSCI441::ShaderProgram( "shaders/asteroid.v.glsl", "shaders/asteroid.f.glsl" );
    asteroidShaderProgramUniforms.mvpMatrix      = asteroidShaderProgram->getUniformLocation("mvpMatrix");
    asteroidShaderProgramUniforms.modelMatrix    = asteroidShaderProgram->getUniformLocation("modelMatrix");
    asteroidShaderProgramUniforms.normalMatrix   = asteroidShaderProgram->getUniformLocation("normalMatrix");
    asteroidShaderProgramUniforms.viewDir        = asteroidShaderProgram->getUniformLocation("viewDir");
    asteroidShaderProgramUniforms.asteroidTex        = asteroidShaderProgram->getUniformLocation("asteroidTex");
    // Point lights uniforms
    asteroidShaderProgramUniforms.pointLights[0].position     = asteroidShaderProgram->getUniformLocation("pointLights[0].position");
    asteroidShaderProgramUniforms.pointLights[0].color        = asteroidShaderProgram->getUniformLocation("pointLights[0].color");
    asteroidShaderProgramUniforms.pointLights[1].position     = asteroidShaderProgram->getUniformLocation("pointLights[1].position");
    asteroidShaderProgramUniforms.pointLights[1].color        = asteroidShaderProgram->getUniformLocation("pointLights[1].color");
    // Directional lights uniforms
    asteroidShaderProgramUniforms.directionalLights[0].position         = asteroidShaderProgram->getUniformLocation("directionalLights.position");
    asteroidShaderProgramUniforms.directionalLights[0].direction     = asteroidShaderProgram->getUniformLocation("directionalLights.direction");
    asteroidShaderProgramUniforms.directionalLights[0].color         = asteroidShaderProgram->getUniformLocation("directionalLights.color");

    asteroidShaderProgramAttributes.vPos         = asteroidShaderProgram->getAttributeLocation("vPos");
    asteroidShaderProgramAttributes.vNormal      = asteroidShaderProgram->getAttributeLocation("vNormal");
    asteroidShaderProgramAttributes.vUV          = asteroidShaderProgram->getAttributeLocation("vUV");


    asteroidShaderProgram->useProgram();                         // set our shader program to be active
    glUniform1i(asteroidShaderProgramUniforms.asteroidTex, 0);

    // load the image data from file to a byte array
    int imageWidth, imageHeight, imageChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load( texturePath, &imageWidth, &imageHeight, &imageChannels, 0);
    const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);

    if( data ) {
        glGenTextures(1, &asteroidTextureHandle);
        glBindTexture(GL_TEXTURE_2D, asteroidTextureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, STORAGE_TYPE, imageWidth, imageHeight, 0, STORAGE_TYPE, GL_UNSIGNED_BYTE, data);

        fprintf( stdout, "[INFO]: %s texture map read in with handle %d\n", texturePath, asteroidTextureHandle);

        stbi_image_free(data);                                  // release the image data from the CPU
    } else {
        // error loading image, alert the user
        fprintf( stderr, "[ERROR]: Could not load texture map \"%s\"\n", texturePath );
    }

    struct VertexTextured {
        float x, y, z;
        float s, t;
    };

    const int stacks = 64;
    const int slices = 64;

    std::vector<VertexTextured> asteroidVertices;
    std::vector<unsigned short> asteroidIndices;

    for (int i = 0; i <= stacks; i++){

        float V   = i / (float)stacks;
        float phi = V * M_PI;

        for (int j = 0; j <= slices; j++){

            float U = j / (float)slices;
            float theta = U * (2 * M_PI);

            // Calc The Vertex Positions
            float x = cosf(theta) * sin(phi);
            float y = cosf(phi);
            float z = sinf(theta) * sin(phi);

            // Push Back Vertex Data
            asteroidVertices.push_back({x, y, z, U, V});
        }
    }

    for (int i = 0; i < slices * stacks + slices; i++){
        asteroidIndices.push_back(i);
        asteroidIndices.push_back(i + slices + 1);
        asteroidIndices.push_back(i + slices);

        asteroidIndices.push_back(i + slices + 1);
        asteroidIndices.push_back(i);
        asteroidIndices.push_back(i + 1);
    }

    printf("Number of vertices:%lu ", asteroidVertices.size());
    printf("Number of indices:%lu ", asteroidIndices.size());

    asteroidVertexCount = asteroidIndices.size();

    glGenVertexArrays(1, &asteroidVAO);
    glBindVertexArray(asteroidVAO);

    glGenBuffers(1, &asteroidVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTextured) * asteroidVertexCount, &asteroidVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray( asteroidShaderProgramAttributes.vPos );
    glVertexAttribPointer( asteroidShaderProgramAttributes.vPos,
                           3, //number of elements in the attribute
                           GL_FLOAT, //data type
                           GL_FALSE, //do not normalize
                           sizeof(VertexTextured), //stride
                           (void*)0); //relative starting address

    glEnableVertexAttribArray( asteroidShaderProgramAttributes.vNormal );
    glVertexAttribPointer( asteroidShaderProgramAttributes.vNormal,
                           3, //number of elements in the attribute
                           GL_FLOAT, //data type
                           GL_FALSE, //do not normalize
                           sizeof(VertexTextured), //stride
                           (void*)0); //relative starting address

    glEnableVertexAttribArray(  asteroidShaderProgramAttributes.vUV );
    glVertexAttribPointer( asteroidShaderProgramAttributes.vUV, // location connection
                           2, //number of elements in the attribute
                           GL_FLOAT, //data type
                           GL_FALSE, //do not normalize
                           sizeof(VertexTextured), //stride
                           (void*)(sizeof(float) * 3)); //relative starting address

    glGenBuffers(1, &asteroidIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asteroidIBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * asteroidVertexCount, &asteroidIndices[0], GL_STATIC_DRAW);

    for (int i = 0; i < initialCount; i ++) { // Make 100 asteroids
        spawnAsteroid();
    }

}


//check for collisions and spawn particles accordingly. As well, checks if the ship has hit an asteroid
GLboolean AsteroidSystem::update(GLfloat timeStep, Ship& ship) {
    for(int i = 0; i < asteroids.size(); i ++){
        asteroids[i].update(timeStep, ship.getVelocity(), domainScale);
        if(asteroids[i].collisionTest(ship.getPosition(), 1.5f)) {
            return true;
        }



        for(int j = 0; j < i; j ++){
            if(asteroids[i].collisionTest(asteroids[j].getPosition())){
                glm::vec3 delta = asteroids[i].getPosition() - asteroids[j].getPosition();

                if(dot(delta, asteroids[i].getVelocity()) < 0.0f) {
                    asteroids[i].bounce(delta);
                    asteroids[j].bounce(delta);
                }
            }
        }

        //check for asteroid collisions with projectiles
        for (int k = 0; k < ship.shots.size(); k ++ ){
            if(asteroids[i].collisionTest(ship.shots[k].getPosition(), 0)) {
                //erase both the projectile and the asteroid
                ship.shots.erase(ship.shots.begin() + k);
                asteroids.erase(asteroids.begin() + i);
                asteroidsDestroyed ++;
                i--;
                break;
            }
        }
    }
}

void AsteroidSystem::draw(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    asteroidShaderProgram->useProgram();

    float angle = 0;

    for (int i = 0; i < asteroids.size(); i++) {

        glm::mat4 modelMatrix = asteroids[i].getTransformation();
        glm::mat4 mvpMtx = projMatrix * viewMatrix * modelMatrix;

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

        glUniformMatrix4fv(asteroidShaderProgramUniforms.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);
        glUniformMatrix4fv(asteroidShaderProgramUniforms.modelMatrix, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix3fv(asteroidShaderProgramUniforms.normalMatrix, 1, GL_FALSE, &normalMatrix[0][0]);

        glBindTexture(GL_TEXTURE_2D, asteroidTextureHandle);
        glBindVertexArray( asteroidVAO );
        glDrawElements( GL_TRIANGLES, asteroidVertexCount, GL_UNSIGNED_SHORT, (void*)0 );
    }
}

void AsteroidSystem::setLightingParameters(Ship &ship, glm::vec3 &camDir) {
    asteroidShaderProgram->useProgram();

    ship.sendLightingData(asteroidShaderProgramUniforms.pointLights[0].position,
                          asteroidShaderProgramUniforms.pointLights[0].color,
                          asteroidShaderProgramUniforms.pointLights[1].position,
                          asteroidShaderProgramUniforms.pointLights[1].color,
                          asteroidShaderProgramUniforms.directionalLights[0].position,
                          asteroidShaderProgramUniforms.directionalLights[0].direction,
                          asteroidShaderProgramUniforms.directionalLights[0].color );
}

void AsteroidSystem::spawnAsteroid() {
    glm::vec3 position = glm::vec3(0);
    while(length(position) < 50){
      position = glm::vec3(randNumber1(domainScale.x / 2.0f ), randNumber1(domainScale.y / 2.0f), randNumber1(domainScale.z / 2.0f));
    }

    glm::vec3 velocity = 30.0f * glm::normalize(glm::normalize(-position) + 2.0f * glm::vec3(randNumber1(1), randNumber1(1), randNumber1(1)));
    glm::vec3 rotationAxis = glm::normalize(glm::vec3(randNumber1(1), randNumber1(1), randNumber1(1)));


    asteroids.push_back(Asteroid(position,  velocity, rotationAxis,  10.0f, randNumber1(1.0f)));
}

GLint AsteroidSystem::getAsteroidsDestroyed() {
    return asteroidsDestroyed;
}

GLboolean AsteroidSystem::gameWon(){
    return (asteroids.size() == 0);
}