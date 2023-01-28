#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 in_color;

layout(push_constant) uniform vert_constants {
	vec4 vert_scale;
};

layout(location = 0) out vec4 out_color;

void main() {
	out_color   = in_color;
	gl_Position = vec4(
		position.x * vert_scale.x,
		position.y * vert_scale.y,
		position.z,
		position.w
	);
}