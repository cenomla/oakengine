#version 330 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vTexCoords;

layout (shared) uniform MatrixBlock {
	mat4 model;
	mat4 view;
	mat4 proj;
} matrix;

out vec2 passTexCoords;

void main() {
	gl_Position = matrix.proj * matrix.view * matrix.model * vec4(vPosition, 0.0, 1.0);
	passTexCoords = vTexCoords;
}

