#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat4 objectMatrix;              // transform of the object in world space

uniform float time;

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
layout(location = 1) in vec3 vNormal;    // the normal of this specific vertex in object space
in vec3 viewVector;

// varying outputs
out vec3 color;    // color to apply to this vertex
out vec3 position;

void main() {
    vec3 worldPos = (objectMatrix * vec4(vPos, 1)).xyz;
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
    position = worldPos;
}