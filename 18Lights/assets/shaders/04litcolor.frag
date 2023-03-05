#version 460

layout(location = 0) in vec4 in_color;

layout(set = 1, binding = 0) uniform LightProps {
    float ambient;
};

layout(location = 0) out vec4 out_color;

void main() {
    out_color = in_color * ambient;
}