#version 460 core

// Here is the matching location and data type for the color output value
// provided by the vertex shader.
layout(location = 0) in vec4 in_color;

// And the same required output color from last time, but with out the fixed
// value.
layout(location = 0) out vec4 out_color;

// Both shader stages are still just passthroughs that do no meaningful work
// to their respective data, but when you combine them into a pipeline, some
// cool stuff happens.
void main() {
    out_color = in_color;
}