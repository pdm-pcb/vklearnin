#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec2 in_uv;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(location = 0) out vec3 out_uvw;

void main() {
	out_uvw = in_pos.xyz;
	gl_Position = proj_mat * view_mat * in_pos;
}