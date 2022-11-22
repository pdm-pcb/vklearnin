#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 vs_out_color;

void main() {
	gl_Position = position;
    vs_out_color = color;
}