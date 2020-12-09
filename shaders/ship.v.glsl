#version 410 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;

out vec3 coords;
out vec2 uv;
out vec3 light;

uniform mat4 normalMatrix;
uniform mat4 mvpMatrix;

void main()
{
    uv = vUV;
    coords = vPos;

    vec3 normal = normalize(vNormal);

    light = vec3(1);

    gl_Position = mvpMatrix * vec4(vPos, 1.0);



}
