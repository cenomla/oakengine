#version 450 core

layout (location = 1) in vec2 vPosition;
layout (location = 3) in vec2 vUV;

layout (std140, binding = 1) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec2 passUV;

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(vPosition, 0.0, 1.0);
	passUV = vUV;
}