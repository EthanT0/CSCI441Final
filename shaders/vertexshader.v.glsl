#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat4 normalMatrix;              // the normal matrix
uniform mat4 objectMatrix;              // transform of the object in world space
uniform vec3 lightDirection;            // the direction the incident ray of light is traveling

uniform vec3 lightColor;                // the color of the light
uniform vec3 materialColor;             // the material color for our vertex (& whole object)

uniform vec3 pointLight4Pos;
uniform vec3 pointLight4Color;

uniform float time;


// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
layout(location = 1) in vec3 vNormal;    // the normal of this specific vertex in object space
in vec3 viewVector;

// varying outputs
out vec3 color;    // color to apply to this vertex

void main() {
    vec3 worldPos = (objectMatrix * vec4(vPos, 1)).xyz;
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    //normal in object space
    vec3 objectNormal = normalize((normalMatrix * vec4(vNormal, 0)).xyz);
    vec3 inverseLightDirection = -normalize(lightDirection);

    // Computing Diffuse for directional light
    vec3 light = lightColor * (max(dot(objectNormal, inverseLightDirection), 0));

    vec3 vertexToPL4 = pointLight4Pos - worldPos;
    vec3 diffuseFromPL4 = pointLight4Color * max(dot(objectNormal, normalize(vertexToPL4)), 0.05) / (1 + 0.03 * dot(vertexToPL4, vertexToPL4));

    light += diffuseFromPL4;

    light += vec3(0.30, 0.30, 0.30); //ambient light

    color = materialColor * light;
    //color = colorLerp * color + (1 - colorLerp) * skyColor;

}