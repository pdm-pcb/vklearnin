#version 460

layout(location = 0) in vec3 in_uvw;

layout(set = 1, binding = 0) uniform samplerCube image_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(image_sampler, in_uvw);
}