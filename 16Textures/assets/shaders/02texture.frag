#version 460 core

layout(location = 0) in vec2 in_uv;

layout(binding = 1) uniform sampler2D image_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(image_sampler, in_uv);
}