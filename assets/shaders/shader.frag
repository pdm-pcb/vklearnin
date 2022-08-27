#version 460 core

layout(binding = 1) uniform sampler2D texsampler;

layout(location = 0) in vec3 vs_out_color;
layout(location = 1) in vec2 vs_out_texcoord;

layout(location = 0) out vec4 ps_out_color;

void main() {
    ps_out_color = texture(texsampler, vs_out_texcoord);
}