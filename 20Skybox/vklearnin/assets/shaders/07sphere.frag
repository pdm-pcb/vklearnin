#version 460 core

layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec4 out_color;

void main() {
    vec2 texcoord = vec2(
        atan(in_pos.z, in_pos.x) / 6.28318530718,
        acos(in_pos.y) / 3.14159265359
    );
    out_color = texture(texture_sampler, texcoord);
}