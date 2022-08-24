#version 460 core

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 vs_out_color;

void main() {
    gl_Position = in_position;
    vs_out_color = in_color;
}