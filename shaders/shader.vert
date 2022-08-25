#version 460 core

layout(binding = 0) uniform MVPMatrices {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 vs_out_color;

void main() {
    gl_Position = proj * view *  model * in_position;
    vs_out_color = in_color;
}