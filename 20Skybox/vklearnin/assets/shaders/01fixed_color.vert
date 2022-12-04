#version 460 core

layout(location = 0) out vec4 vs_out_color;

void main() {
	const vec4 vertices[3] = vec4[3](
		vec4( 0.0,  0.5, 0.0, 1.0),
		vec4(-0.5, -0.5, 0.0, 1.0),
		vec4( 0.5, -0.5, 0.0, 1.0)
	);

	const vec4 colors[3] = vec4[3](
		vec4(0.25, 0.0,  0.0,  1.0),
		vec4(0.0,  0.25, 0.0,  1.0),
		vec4(0.0,  0.0,  0.25, 1.0)
	);

	gl_Position = vertices[gl_VertexIndex];
    vs_out_color = colors[gl_VertexIndex];
}