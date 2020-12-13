#version 410 core

out vec4 FragColor;

in vec3 localPosition;
uniform float time;

uniform vec3 color;

void main()
{
    float timeOffset = time + sin(localPosition.x * 20) + sin(localPosition.y * 10) + sin(localPosition.z * 10);
    FragColor = vec4(1);
    FragColor.rgb = mix(vec3(1, 1, 1), vec3(0, 1, 1), 0.5 * (sin(timeOffset) + 1));
    FragColor.rgb = mix(vec3(0.5, 0.5, 0.5), FragColor.rgb, localPosition.x);
}