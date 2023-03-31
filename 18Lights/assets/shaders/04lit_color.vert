#version 460

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec3 out_pos;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec4 out_pos_dir_light_space;
layout(location = 4) out vec4 out_pos_spot_light_space;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(set = 2, binding = 0) uniform ShadowMapTransforms {
	mat4 dir_vp_mat;
    mat4 spot_vp_mat;
};

layout(push_constant) uniform ModelPush {
	mat4 model_mat;
};

const mat4 vulkan_ndc_bias = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

void main() {
	vec4 world_pos = model_mat * in_pos;

	out_color  = in_color;
	out_pos    = world_pos.xyz;
	out_normal = normalize(mat3(model_mat) * in_normal.xyz);

	out_pos_dir_light_space = vulkan_ndc_bias * dir_vp_mat * world_pos;
	out_pos_spot_light_space = vulkan_ndc_bias * spot_vp_mat * world_pos;

	gl_Position = proj_mat * view_mat * world_pos;
}