/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 410 core

in vec2 uv;
in float particleDistance;

uniform vec3 skyColor;

uniform sampler2D image;

out vec4 fragColorOut;

void main() {

    float distanceFalloff = 1 / exp(particleDistance / 30);

    vec4 texelColor = texture(image, uv);

    vec3 finalColor = mix(skyColor, texelColor.rgb, distanceFalloff);

    fragColorOut = vec4(finalColor, texelColor.a);

}
