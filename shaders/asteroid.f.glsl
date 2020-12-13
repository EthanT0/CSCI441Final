#version 410 core

out vec4 FragColor;

in vec3 coords;
in vec2 uv;

in vec3 light;

uniform sampler2D asteroidTex;

void main()
{
    FragColor = vec4((light + 0.03f) * texture(asteroidTex, uv).rgb, 1);
}