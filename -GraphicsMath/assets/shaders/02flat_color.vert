#version 460 core

// While the descriptions in the Vertex class specify a binding of zero along
// with a location of zero, we only need to specify a location here. Being
// explicit about both the binding and location will come with later
// improvements to our shader code.
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 in_color;

// We're now specifying an optional output of the vertex shader. Since the
// vertex data is providing a color for us, we want to pass it along to
// the fragment shader.
layout(location = 0) out vec4 out_color;

void main() {
	mat4 view = mat4(
		1.000000,  -0.000000,   0.000000,   0.000000,
		0.000000,   1.000000,   0.000000,   0.000000,
		0.000000,   0.000000,  -1.000000,   0.000000,
		0.000000,   0.000000,   2.000000,   1.000000
	);

	mat4 proj = mat4(
		0.562793,   0.000000,   0.000000,   0.000000,
		0.000000,   1.000000,   0.000000,   0.000000,
		0.000000,   0.000000,   1.000100,   1.000000,
		0.000000,   0.000000,  -0.100010,   0.000000
	);

	out_color   = in_color;
	gl_Position = proj * view * position;
}