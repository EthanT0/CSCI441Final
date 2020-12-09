//
// Created by ethan on 12/6/20.
//

#include "Skybox.h"

Skybox::Skybox() {}

Skybox::Skybox(char** skyboxPathways) {
    skyboxShader = new CSCI441::ShaderProgram( "shaders/skybox.v.glsl", "shaders/skybox.f.glsl" );
    skyboxShaderUniforms.mvpMatrix      = skyboxShader->getUniformLocation("mvpMatrix");
    skyboxShaderUniforms.skybox = skyboxShader->getUniformLocation("skybox");

    skyboxShaderAttributes.vPos         = skyboxShader->getAttributeLocation("vPos");


    glGenTextures(1, &skyboxTextureHandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureHandle);
    int width, height, channels;
    for(int i = 0; i < 6; i ++ ) {
        unsigned char *data = stbi_load( skyboxPathways[i], &width, &height, &channels, 0);
        if(data) {
            fprintf(stdout, "Loaded cubemap texture [%s]\n", skyboxPathways[i] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else {
            fprintf(stdout, "Cubemap texture failed to load [%s]\n", skyboxPathways[i] );
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    // create our platform
    glm::vec3 cubeVertices[8] = {
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f,  -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f,  -1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f,  1.0f, 1.0f)
    };
    unsigned short cubeIndices[36] = {0, 1, 2, 1, 2, 3,
                                      4, 5, 6, 5, 6, 7,
                                      0, 2, 4, 2, 4, 6,
                                      1, 3, 5, 3, 5, 7,
                                      0, 1, 4, 1, 4, 5,
                                      2, 3, 6, 3, 6, 7};

    glGenVertexArrays( 1, &skyboxVAO );
    glBindVertexArray( skyboxVAO );

    glGenBuffers( 1, &skyboxVBO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), cubeVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( skyboxShaderAttributes.vPos );
    glVertexAttribPointer( skyboxShaderAttributes.vPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glGenBuffers(1, &skyboxIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 36, &cubeIndices[0], GL_STATIC_DRAW);
}

void Skybox::draw(glm::mat4 rotationMtx){
    skyboxShader->useProgram();

    glDepthMask(GL_FALSE);

    glUniformMatrix4fv(skyboxShaderUniforms.mvpMatrix, 1, GL_FALSE, &rotationMtx[0][0] );


    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureHandle);
    glBindVertexArray( skyboxVAO );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0 );

    glDepthMask(GL_TRUE);
}
