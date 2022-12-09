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
    vec3 incident   = normalize(vert_pos.xyz - camera_pos.xyz);
    vec3 normal     = normalize(vert_normal.xyz);
    vec3 reflection = reflect(incident, normal);

    float refract_index = 1.762;
    vec3 refraction = refract(incident, normal, 1.0 / refract_index);

    vec3 reflection_sample = texture(texture_sampler, reflection).rgb;
    vec3 refraction_sample = texture(texture_sampler, refraction).rgb;

    // final_color = vec4(reflection_sample, 1.0);
    // final_color = vec4(refraction_sample, 1.0);
    final_color = vec4(mix(reflection_sample, refraction_sample, 0.66), 1.0);
}