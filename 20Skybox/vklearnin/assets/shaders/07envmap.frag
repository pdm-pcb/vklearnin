#version 460 core

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_matrix;
    mat4 proj_matrix;
    vec4 camera_pos;
};

layout(set = 1, binding = 0) uniform samplerCube texture_sampler;

layout(location = 0) in vec4 vert_pos;
layout(location = 1) in vec4 vert_normal;

layout(location = 0) out vec4 final_color;

void main() {
    vec3 incident = normalize(vert_pos.xyz - camera_pos.xyz);
    vec3 reflection = reflect(incident, normalize(vert_normal.xyz));

    final_color = vec4(texture(texture_sampler, reflection).rgb, 1.0);
}