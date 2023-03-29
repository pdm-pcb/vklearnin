#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec4 in_color;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec3 out_pos;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec4 out_light_space_pos;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(set = 2, binding = 0) uniform LightVPMatrix {
    mat4 light_vp_matrix;
};

layout(push_constant) uniform ModelPush {
	mat4 model_mat;
};

void main() {
	vec4 world_pos = model_mat * in_pos;

	out_color  = in_color;
	out_pos    = world_pos.xyz;
	out_normal = normalize(mat3(model_mat) * in_normal.xyz);

	out_light_space_pos = light_vp_matrix * in_pos;

	gl_Position = proj_mat * view_mat * world_pos;
}