#version 330 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vTexCoords;

layout (shared) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec2 passTexCoords;

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(vPosition, 0.0, 1.0);
	passTexCoords = vTexCoords;
}

