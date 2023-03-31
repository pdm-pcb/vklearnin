#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec2 in_uv;

layout(push_constant) uniform ShadowPassMVP {
    mat4 light_vp_matrix;
    mat4 model_matrix;
};

void main() {
    gl_Position = light_vp_matrix * model_matrix * in_pos;
}