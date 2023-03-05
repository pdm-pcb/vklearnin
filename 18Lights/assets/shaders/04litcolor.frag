#version 460

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;

struct DirectionalLight {
    vec4 toward;
    vec4 color;
};

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 1, binding = 0) uniform LightProps {
    DirectionalLight dir;
    PointLight point;
    float ambient;
} lights;

layout(location = 0) out vec4 out_color;

vec3 calc_directional_light(DirectionalLight light);
vec3 calc_point_light(PointLight light);

void main() {
    vec3 directional = calc_directional_light(lights.dir);
    vec3 point = calc_point_light(lights.point);

    out_color = vec4(directional + point, in_color.w);
}

vec3 calc_directional_light(DirectionalLight light) {
    float dir_intensity = max(
        dot(in_normal, light.toward.xyz),
        0.0
    ) * light.color.w;

    vec3 ambient = light.color.rgb * lights.ambient * in_color.rgb;
    vec3 diffuse = light.color.rgb * dir_intensity * in_color.rgb;

    return ambient + diffuse;
}

vec3 calc_point_light(PointLight light) {
    // a vector dotted with itself is its length squared, so this gives us the
    // inverse square law quickly and cheaply
    vec3 to_light = light.position.xyz - in_pos.xyz;
    float attenuation = 1.0 / dot(to_light, to_light);

    float point_intensity = max(
        dot(normalize(in_normal), normalize(to_light)),
        0.0
    ) * light.color.w * attenuation;

    vec3 ambient = light.color.rgb * lights.ambient * in_color.rgb;
    vec3 diffuse = light.color.rgb * point_intensity * in_color.rgb;

    return ambient + diffuse;
}