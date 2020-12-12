//
// Created by evaughan on 12/10/20.
//

#include "AsteroidSystem.h"

GLfloat randNumber1( GLfloat max ) {
    return rand() / (GLfloat)RAND_MAX * max * 2.0 - max;
}

AsteroidSystem::AsteroidSystem(){}

AsteroidSystem::AsteroidSystem(char* texturePath) {
    asteroidTextureHandle = 0;

    asteroidShaderProgram = new CSCI441::ShaderProgram( "shaders/asteroid.v.glsl", "shaders/asteroid.f.glsl" );
    asteroidShaderProgramUniforms.mvpMatrix   = asteroidShaderProgram->getUniformLocation("mvpMatrix");
    asteroidShaderProgramAttributes.vPos      = asteroidShaderProgram->getAttributeLocation("vPos");
    asteroidShaderProgramUniforms.textureMap = asteroidShaderProgram->getUniformLocation("textureMap");
    asteroidShaderProgramAttributes.tPosition = asteroidShaderProgram->getAttributeLocation("tPosition");

    asteroidShaderProgram->useProgram();                         // set our shader program to be active
    glUniform1i(asteroidShaderProgramUniforms.textureMap, 0);

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



    const int stacks = 10;
    const int slices = 10;

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
            asteroidVertices.push_back({x, y, z, 0, 0});
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
//    for (int i = 0; i < sizeof(asteroidVertices); i++) {
//        printf("x: ", asteroidVertices[i].x);
//        printf("y: ", asteroidVertices[i].y);
//        printf("z: ", asteroidVertices[i].z);
//
//
//    }
    printf("Number of indices:%lu ", asteroidIndices.size());
    VertexTextured* v = &asteroidVertices[0];
    unsigned short* i = &asteroidIndices[0];

    asteroidShaderProgram->useProgram();

    glGenVertexArrays( 1, &asteroidVAO );
    glBindVertexArray( asteroidVAO );

    glGenBuffers( 2, asteroidVBOs );

    glBindBuffer( GL_ARRAY_BUFFER, asteroidVBOs[0] );
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW );
    glEnableVertexAttribArray( asteroidShaderProgramAttributes.vPos );
    glVertexAttribPointer( asteroidShaderProgramAttributes.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );

    glEnableVertexAttribArray( asteroidShaderProgramAttributes.tPosition );
    glVertexAttribPointer( asteroidShaderProgramAttributes.tPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)(sizeof(float) * 3) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, asteroidVBOs[1] );
    glBufferData(GL_ARRAY_BUFFER, sizeof(i), i, GL_STATIC_DRAW );
    CSCI441::setVertexAttributeLocations(asteroidShaderProgramAttributes.vPos,   // vertex position location
                                         -1,                   // vertex normal location not used, set to -1
                                         asteroidShaderProgramAttributes.tPosition );

    fprintf( stdout, "[INFO]: quad read in with VAO %d\n\n", asteroidVAO );

    CSCI441::setVertexAttributeLocations(asteroidShaderProgramAttributes.vPos,   // vertex position location
                                         -1,                   // vertex normal location not used, set to -1
                                         asteroidShaderProgramAttributes.tPosition );

    for (int i = 0; i < 100; i ++) { // Make 100 asteroids
        Asteroid asteroid = Asteroid(glm::vec3(randNumber1(20), randNumber1(20), randNumber1(20)), // Initial position
                                     glm::vec3(1, 1, 1),  // Axis of rotation
                                     glm::vec3(10, 10, 10)); // Scale

        asteroids.push_back(asteroid);
        translations.push_back(glm::vec3(randNumber1(2), randNumber1(2), randNumber1(2))); // Generate random initial translation vector
    }

}

void AsteroidSystem::draw(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    asteroidShaderProgram->useProgram();

    float angle = 0;

    float translationScalar = 0.01f;

    for (int i = 0; i < asteroids.size(); i++) {

        glm::vec3 translation = translations[i] * translationScalar;

        glm::mat4 mvpMtx = asteroids[i].draw(viewMatrix, projMatrix, translation, angle);
        glUniformMatrix4fv(asteroidShaderProgramUniforms.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);

        glBindTexture(GL_TEXTURE_2D, asteroidTextureHandle);
        glBindVertexArray( asteroidVAO );
        glDrawElements( GL_TRIANGLE_STRIP, 24, GL_UNSIGNED_SHORT, (void*)0 );
    }

}


void sendMvpMatrix(){

}