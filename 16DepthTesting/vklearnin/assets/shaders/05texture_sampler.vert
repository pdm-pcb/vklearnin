#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texcoord;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view_matrix;
    mat4 proj_matrix;
};

layout(push_constant) uniform InstanceUBO {
    mat4 model_matrix;
    int material_index;
};

layout(location = 0) out vec4 vs_out_color;
layout(location = 1) out vec2 vs_out_texcoord;

void main() {
	gl_Position = proj_matrix * view_matrix * model_matrix * position;

    vs_out_color = color;
    vs_out_texcoord = texcoord;
}