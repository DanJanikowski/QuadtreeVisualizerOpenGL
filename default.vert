#version 460 core

// Inputs
layout (location = 0) in vec2 aPos;

void main() {
   gl_Position = vec4(vec3(aPos, 0), 1.0);
}