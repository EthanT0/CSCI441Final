/*
 *   Geometry Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 410 core

layout(points) in;

layout(triangle_strip, max_vertices = 4) out;


uniform mat4 projMatrix;

out vec2 uv;
out float particleDistance;

void main() {

    float centerDistance = length((projMatrix * gl_in[0].gl_Position).xyz);

    float size = 0.2 * (1 - smoothstep(0.0, 60.0, centerDistance)) + 0.05;


    gl_Position = projMatrix * (gl_in[0].gl_Position + vec4(vec2(-1, -1) * size, 0, 0));
    particleDistance = length(gl_Position.xyz);
    uv = vec2(0, 1);
    EmitVertex();

    gl_Position = projMatrix * (gl_in[0].gl_Position + vec4(vec2(-1, 1) * size, 0, 0));
    uv = vec2(0, 0);
    particleDistance = length(gl_Position.xyz);
    EmitVertex();

    gl_Position = projMatrix * (gl_in[0].gl_Position + vec4(vec2(1, -1) * size, 0, 0));
    uv = vec2(1, 1);
    particleDistance = length(gl_Position.xyz);
    EmitVertex();

    gl_Position = projMatrix * (gl_in[0].gl_Position + vec4(vec2(1, 1) * size, 0, 0));
    uv = vec2(1, 0);
    particleDistance = length(gl_Position.xyz);
    EmitVertex();

    EndPrimitive();
}
