#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;

layout (shared) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
	mat4 model;
} matrix;

out vec2 passTexCoords;

void main() {
	gl_Position = matrix.proj * matrix.view * matrix.model * vec4(vPosition, 1.0);
	passTexCoords = vTexCoords;
}

