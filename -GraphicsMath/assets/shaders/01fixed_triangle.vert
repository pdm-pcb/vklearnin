#version 460 // GLSL version identifier

// Three vertices to describe a triangle in space
const vec4 vertices[3] = vec4[3](
    vec4(-0.5f, -0.5f, 0.0f, 1.0f), // -x, -y is the left corner
    vec4( 0.0f,  0.5f, 0.0f, 1.0f), // zero, +y is the tip of the triangle
    vec4( 0.5f, -0.5f, 0.0f, 1.0f)  // +x, -y is the right corner
);

void main() {
    // The vertex shader only has one required output: the position of the
    // vertex it just completed processing. Normally, there's some more 3D math
    // that goes on before assigning  this value, but this is where we're
    // starting.
	gl_Position = vertices[gl_VertexIndex];
}