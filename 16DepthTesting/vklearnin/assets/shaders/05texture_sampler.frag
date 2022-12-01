#version 460 core

layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

layout(location = 0) in vec4 vs_out_color;
layout(location = 1) in vec2 vs_out_texcoord;

layout(location = 0) out vec4 ps_out_color;

void main() {
    ps_out_color = texture(texture_sampler, vs_out_texcoord);
    // ps_out_color = vs_out_color;
}