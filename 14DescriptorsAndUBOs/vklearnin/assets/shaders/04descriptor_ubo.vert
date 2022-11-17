#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

layout(push_constant) uniform CameraData {
    mat4 view_matrix;
    mat4 proj_matrix;
} camera_data;

layout(binding = 0) uniform InstanceData {
    mat4 model_matrix;
} instance_ubo;

layout(location = 0) out vec4 vs_out_color;

void main() {
	gl_Position = camera_data.proj_matrix   *
                  camera_data.view_matrix   *
                  instance_ubo.model_matrix *
                  position;
    vs_out_color = color;
}