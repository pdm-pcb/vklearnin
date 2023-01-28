#version 460 core

layout(location = 0) in vec4 in_color;

layout(push_constant) uniform frag_constants {
	layout(offset = 16) vec4 color_scale;
};

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(
        in_color.r * color_scale.r,
        in_color.g * color_scale.g,
        in_color.b * color_scale.b,
        in_color.a
    );
}