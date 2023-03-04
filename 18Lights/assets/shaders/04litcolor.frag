#version 460

layout(location = 0) in vec4 in_color;

// layout(set = 1, binding = 0) uniform PointLight {
//     float ambient;
// } point_light;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = in_color;
}