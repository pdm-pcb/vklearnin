#version 460

layout(location = 0) in vec4 in_pos;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

struct DirectionalLight {
    vec4 toward;
    vec4 color;
};

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 1, binding = 0) uniform LightData {
    DirectionalLight dir;
    PointLight point;
    float scene_ambient;
} lights;

layout(push_constant) uniform ModelPush {
    mat4 model_mat;
};

void main() {
    mat4 dir_light_view = view_mat;
    dir_light_view[3] = lights.dir.toward * 25.0;
    gl_Position = proj_mat * dir_light_view * model_mat * in_pos;
}