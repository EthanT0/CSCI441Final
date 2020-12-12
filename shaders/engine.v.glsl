#version 410 core

layout (location = 0) in vec3 vPos;

out vec3 localPosition;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
    localPosition = vPos;
}
