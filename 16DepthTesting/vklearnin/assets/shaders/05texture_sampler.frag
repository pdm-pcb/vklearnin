#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler2D texture_sampler;

layout(push_constant) uniform InstanceUBO {
    mat4 model_matrix;
    int material_index;
};

layout(location = 0) in vec4 vs_out_color;
layout(location = 1) in vec2 vs_out_texcoord;

layout(location = 0) out vec4 ps_out_color;

void main() {
    ps_out_color = texture(texture_sampler, vs_out_texcoord);
}