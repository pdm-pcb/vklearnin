#version 460 core

layout(set = 0, binding = 0) uniform VPMatrices {
    mat4 view;
    mat4 proj;
};

layout(push_constant) uniform ModelMatrix {
    mat4 model;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0) out vec3 vs_out_color;
layout(location = 1) out vec2 vs_out_texcoord;

void main() {
    gl_Position = proj * view *  model * vec4(in_position, 1.0);
    vs_out_color = in_color;
    vs_out_texcoord = in_texcoord;
}