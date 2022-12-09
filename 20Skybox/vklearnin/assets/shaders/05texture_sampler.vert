#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec4 bitangent;
layout(location = 4) in vec2 texcoord;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_matrix;
    mat4 proj_matrix;
    vec3 camera_pos;
};

layout(push_constant) uniform InstanceData {
    mat4 model_matrix;
};

layout(location = 0) out vec2 vs_out_texcoord;

void main() {
    vs_out_texcoord = texcoord;
	gl_Position = proj_matrix * view_matrix * model_matrix * position;
}