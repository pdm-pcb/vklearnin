#version 460

layout(location = 0) in vec4 in_position;
layout(location = 2) in vec2 in_uv_coord;

layout(location = 0) out vec2 out_uv;

layout(push_constant) uniform PushConstants {
    mat4 model_mat;
};

layout(set = 0, binding = 0) uniform CameraMatrices {
    mat4 view_mat;
    mat4 proj_mat;
};

void main() {
    gl_Position = proj_mat * view_mat * model_mat * in_position;
    out_uv = in_uv_coord;
}
