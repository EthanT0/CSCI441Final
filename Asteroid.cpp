
#include "Asteroid.h"
Asteroid::Asteroid() {}
Asteroid::Asteroid(char* texturePath, glm::vec3 initialPosition, glm::vec3 rotAxis, glm::vec3 scale) {
    translationVector = initialPosition;
    rotationAngle = 0;
    rotationAxis = rotAxis;
    scaleVector = scale;
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

    float X = 0.5f;
    float Z = 1.0f;

    VertexTextured asteroidVertices[] = {
            {0.019649, 0.840808, 0.448559},
            {0.828664, 0.531788, -0.051401},
            {0.019649, 0.840809, -0.551401},
            {-0.480351, 0.031790, 0.757579},
            {0.019649, -0.777226, 0.448559},
            {0.828664, -0.468209, -0.051401},
            {-0.789368, 0.531788, -0.051401},
            {-0.480351, 0.031791, -0.860501},
            {0.019649, -0.777226, -0.551401},
            {-0.789368, -0.468209, -0.051401},
            {0.519654, 0.031790, 0.757579},
            {0.519654, 0.031791, -0.860501}
    };

    unsigned short asteroidIndices[] = {0, 4, 1, 0, 9, 4, 9, 5, 4, 4, 5, 8, 4, 8, 1,
                                        8, 10, 1, 8, 3, 10, 5, 3, 8, 5, 2, 3, 2, 7, 3,
                                        7, 10, 3, 7, 6, 10, 7, 11, 6, 11, 0, 6, 0, 1, 6,
                                        6, 1, 10, 9, 0, 11, 9, 11, 2, 9, 2, 5, 7, 2, 11 };

    glGenVertexArrays( 1, &asteroidVAO );
    glBindVertexArray( asteroidVAO );

    glGenBuffers( 2, asteroidVBOs );

    glBindBuffer( GL_ARRAY_BUFFER, asteroidVBOs[0] );
    glBufferData(GL_ARRAY_BUFFER, sizeof( asteroidVertices ), asteroidVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( asteroidShaderProgramAttributes.vPos );
    glVertexAttribPointer( asteroidShaderProgramAttributes.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );

    glEnableVertexAttribArray( asteroidShaderProgramAttributes.tPosition );
    glVertexAttribPointer( asteroidShaderProgramAttributes.tPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)(sizeof(float) * 3) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, asteroidVBOs[1] );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof( asteroidIndices ), asteroidIndices, GL_STATIC_DRAW );
        CSCI441::setVertexAttributeLocations(asteroidShaderProgramAttributes.vPos,   // vertex position location
                                         -1,                   // vertex normal location not used, set to -1
                                         asteroidShaderProgramAttributes.tPosition );

    fprintf( stdout, "[INFO]: quad read in with VAO %d\n\n", asteroidVAO );
}

void Asteroid::draw(glm::mat4 viewMtx, glm::mat4 projMtx, glm::vec3 translation, float rotAngle) {
    asteroidShaderProgram->useProgram();
    translationVector = translation;
    rotationAngle = rotAngle;

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translationVector);
    modelMatrix = glm::rotate( modelMatrix, rotationAngle, rotationAxis );
    modelMatrix = glm::scale(modelMatrix, scaleVector);
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMatrix;
    glUniformMatrix4fv(asteroidShaderProgramUniforms.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);

    glBindTexture(GL_TEXTURE_2D, asteroidTextureHandle);
    glBindVertexArray( asteroidVAO );
    glDrawElements( GL_TRIANGLE_STRIP, 24, GL_UNSIGNED_SHORT, (void*)0 );
}

glm::vec3 Asteroid::getPosition() {

    return translationVector;

}