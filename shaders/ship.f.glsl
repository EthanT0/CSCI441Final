#version 410 core

out vec4 FragColor;

in vec3 coords;
in vec2 uv;

in vec3 light;

uniform sampler2D shipTex;

void main()
{
    FragColor = vec4(light * texture(shipTex, uv).rgb, 1);
}