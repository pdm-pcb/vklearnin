#version 460 // GLSL version identifier

// The zero-th output value of the fragment shader (and the only output this
// stage is required to produced) is the final color of the fragment.
layout (location = 0) out vec4 out_color;

void main() {
	// Just like the fixed vertices of the vertex shader, for simplicity's sake
	// we're going with a fixed fragment color. It seems silly to fix the
	// programmable stages of a pipeline, but we'll come back and spruce these
	// up before long.
	out_color = vec4(0.15f, 0.75f, 0.33f, 1.0f);
}