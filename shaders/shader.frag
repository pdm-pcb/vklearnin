#version 460 core

layout(location = 0) in  vec3 vs_out_color;
layout(location = 0) out vec4 ps_out_color;

void main() {
    ps_out_color = vec4(vs_out_color, 1.0);
}