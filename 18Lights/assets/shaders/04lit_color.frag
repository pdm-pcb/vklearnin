#version 460

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec4 in_pos_dir_light_space;
layout(location = 4) in vec4 in_pos_spot_light_space;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

struct DirectionalLight {
    vec4 position;
    vec4 color;
};

struct PointLight {
    vec4 position;
    vec4 color;
};

struct SpotLight {
    vec4 position;
    vec4 forward;
    vec4 color;

    float inner_beam_angle;
    float outer_beam_angle;

    int padding0;
    int padding1;
};

layout(set = 1, binding = 0) uniform LightData {
    DirectionalLight dir;
    PointLight point;
    SpotLight spot;
    float scene_ambient;

    int padding0;
    int padding1;
    int padding2;
} lights;

layout(set = 3, binding = 0) uniform sampler2DShadow dir_shadow_map;
layout(set = 3, binding = 1) uniform sampler2DShadow spot_shadow_map;

vec3 calc_directional_light(DirectionalLight light, vec3 frag_normal,
                            vec3 to_camera);

vec3 calc_point_light(PointLight light, vec3 frag_normal, vec3 to_camera);

vec3 calc_spot_light(SpotLight light, vec3 frag_normal, vec3 to_camera);

float blinn_specular(vec3 to_light, vec3 to_camera, vec3 frag_normal);

void main() {
    vec3 frag_normal = normalize(in_normal);
    vec3 to_camera = normalize(view_mat[3].xyz - in_pos);

    vec3 directional = calc_directional_light(
        lights.dir,
        frag_normal,
        to_camera
    );

    vec3 point = vec3(0.0, 0.0, 0.0);
    // vec3 point = calc_point_light(
    //     lights.point,
    //     frag_normal,
    //     to_camera
    // );

    vec3 spot = vec3(0.0, 0.0, 0.0);
    // vec3 spot = calc_spot_light(
    //     lights.spot,
    //     frag_normal,
    //     to_camera
    // );

    vec3 light_sum = directional + point + spot;

    out_color = vec4(in_color.rgb * light_sum, 1.0);
}

float shadow_factor(vec4 pos_light_space, sampler2DShadow shadow_map) {
    vec3 pos_ndc = pos_light_space.xyz / pos_light_space.w;
    return texture(shadow_map, pos_ndc);
}

vec3 calc_directional_light(DirectionalLight light, vec3 frag_normal,
                            vec3 to_camera)
{
    // Ambient light is always present, so...
    vec3 ambient  = light.color.rgb * lights.scene_ambient;
    vec3 diffuse  = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // The directional light's intensity is color and brightness
    vec3 dir_intensity = light.color.rgb * light.color.w;

    vec3 to_light = normalize(light.position.xyz);

    // Cosine of the angle of incidence, but don't let it go negative. If the
    // dot of the two vectors is negative, the light's angle of icidence is
    // greater than 90 degrees to this fragment's normal, meaning the light is
    // either perfectly parallel or behind this fragment.
    float cos_theta = max(dot(to_light, frag_normal), 0.0);

    // The diffuse component can now account for all factors
    diffuse = dir_intensity * cos_theta;

    // // Use blinn reflectance in order to support camera-to-reflection angles of
    // // more than 90 degrees
    // float blinn = blinn_specular(to_light, to_camera, frag_normal);
    // specular = dir_intensity * blinn;

    float shadow = shadow_factor(in_pos_dir_light_space, dir_shadow_map);
    return ambient + (diffuse + specular) * shadow;
}

vec3 calc_point_light(PointLight light, vec3 frag_normal, vec3 to_camera) {
    // Ambient light is always present, so...
    vec3 ambient  = light.color.rgb * lights.scene_ambient;
    vec3 diffuse  = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // A vector dotted with itself gives its length squared, so these two lines
    // give us the inverse square law quickly and cheaply
    vec3 to_light = light.position.xyz - in_pos.xyz;
    float attenuation = 1.0 / dot(to_light, to_light);

    // The point light's affect accounts for color, brightness, and distance
    vec3 point_intensity = light.color.rgb * light.color.w * attenuation;

    // Now that we've gotten attenuation, normalize to_light for use with other
    // normalized vectors
    to_light = normalize(to_light);

    // Cosine of the angle of incidence, but don't let it go negative. If the
    // dot of the two vectors is negative, the light's angle of icidence is
    // greater than 90 degrees to this fragment's normal, meaning the light is
    // either perfectly parallel or behind this fragment.
    float cos_theta = max(dot(to_light, frag_normal), 0.0);

    // The diffuse component can now account for all factors
    diffuse = point_intensity * cos_theta;

    // // Use blinn reflectance in order to support camera-to-reflection angles of
    // // more than 90 degrees
    // float blinn = blinn_specular(to_light, to_camera, frag_normal);
    // specular = point_intensity * blinn;

    return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 frag_normal, vec3 to_camera) {
    // Ambient light is always present, so...
    vec3 ambient  = light.color.rgb * lights.scene_ambient;
    vec3 diffuse  = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // A vector dotted with itself gives its length squared, so these two lines
    // give us the inverse square law quickly and cheaply
    vec3 to_light = light.position.xyz - in_pos.xyz;
    float attenuation = 1.0 / dot(to_light, to_light);

    vec3 spot_intensity = light.color.rgb * light.color.w * attenuation;

    // Now that we've gotten attenuation, normalize to_light for use with other
    // normalized vectors
    to_light = normalize(to_light);

    float theta = dot(to_light, -light.forward.xyz);
    float cone_epsilon = light.inner_beam_angle - light.outer_beam_angle;
    float cone_attenuation = (theta - light.outer_beam_angle) / cone_epsilon;
    cone_attenuation = clamp(cone_attenuation, 0.0, 1.0);

    // The diffuse component can now account for all factors
    diffuse = spot_intensity * cone_attenuation;

    // // Use blinn reflectance in order to support camera-to-reflection angles of
    // // more than 90 degrees
    // float blinn = blinn_specular(to_light, to_camera, frag_normal);
    // specular = point_intensity * blinn;

    float shadow = shadow_factor(in_pos_spot_light_space, spot_shadow_map);
    return ambient + (diffuse + specular) * shadow;
}

float blinn_specular(vec3 to_light, vec3 to_camera, vec3 frag_normal) {
    // Get the vector half-way between the camera and light incidence vectors
    vec3 half_vector = normalize(to_light - to_camera);
    // Use the half-angle vector with the material specular exponent to get a
    // more versatile highlight calcuation
    float blinn = clamp(dot(frag_normal, half_vector), 0.0, 1.0);
    blinn = pow(blinn, in_color.w);

    return blinn;
}