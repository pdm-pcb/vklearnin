#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 in_color;

layout(binding = 0) uniform CamData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(push_constant) uniform vert_constants {
	vec4 vert_scale;
};

layout(location = 0) out vec4 out_color;

void main() {
	out_color = in_color;

	// mat4 model_mat = mat4(
	// 	0.7071069, -0.0000000,  0.7071066, 0.0f,
	// 	0.4999999,  0.7071069, -0.5000000, 0.0f,
	// 	-0.5000000,  0.7071066,  0.5000002, 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f
	// );

	// mat4 model_mat = mat4(
	// 	0.7071069, -0.0000000, -0.7071066, 0.0f,
	// 	0.4999999,  0.7071069,  0.5000000, 0.0f,
	// 	0.5000000, -0.7071066,  0.5000002, 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f
	// );

	gl_Position = proj_mat * view_mat * vec4(
		position.x * vert_scale.x,
		position.y * vert_scale.y,
		position.z,
		position.w
	);
}