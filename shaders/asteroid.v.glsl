#version 410 core

uniform mat4 mvpMatrix;

layout(location = 0) in vec3 vPos;
in vec2 tPosition;

out vec2 texPos;

void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    texPos = tPosition;

}