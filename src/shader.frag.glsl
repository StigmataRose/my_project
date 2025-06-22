// src/shader_test.frag.glsl
#version 330 core

// No uniforms are needed for this test.

// The only output is the final color of the pixel.
out vec4 out_color;

void main() {
    // Set the output color to a hardcoded value.
    // Magenta (Red=1, Green=0, Blue=1) is a great debug color
    // because it's bright and unlikely to be a default color.
    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}