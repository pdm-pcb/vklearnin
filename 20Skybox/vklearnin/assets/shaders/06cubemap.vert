#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_matrix;
    mat4 proj_matrix;
    vec3 camera_pos;
};

layout(push_constant) uniform InstanceData {
    mat4 model_matrix;
};

layout(location = 0) out vec3 out_texcoord;

void main() {
	gl_Position = proj_matrix * view_matrix * model_matrix * position;

    out_texcoord = position.xyz;
}