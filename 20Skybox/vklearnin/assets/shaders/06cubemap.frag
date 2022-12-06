#version 460 core

layout(set = 1, binding = 0) uniform samplerCube texture_sampler;

layout(location = 0) in vec3 in_texcoord;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(texture_sampler, in_texcoord);
}