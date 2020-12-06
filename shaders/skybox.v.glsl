#version 410 core

layout (location = 0) in vec3 vPos;

out vec3 TexCoords;

uniform mat4 mvpMatrix;

void main()
{
    TexCoords = vPos;
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
}
