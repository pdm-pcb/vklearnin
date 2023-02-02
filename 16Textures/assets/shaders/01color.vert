#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;

layout(binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(push_constant) uniform vert_constants {
	mat4 model_mat;
};

layout(location = 0) out vec4 out_color;

void main() {
	out_color = in_color;
	gl_Position = proj_mat * view_mat * model_mat * in_pos;
}