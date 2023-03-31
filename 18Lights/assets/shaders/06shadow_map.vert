#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec2 in_uv;

layout(set = 0, binding = 0) uniform LightVPMatrices {
    mat4 light_vp_matrix;
};

layout(push_constant) uniform ModelPush {
    mat4 model_mat;
};

void main() {
    gl_Position = light_vp_matrix * model_mat * in_pos;
}