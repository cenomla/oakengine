#version 450 core

layout (location = 1) in vec2 vPosition;
layout (location = 4) in vec4 vColor;

layout (std140, binding = 1) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec4 passColor;

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(vPosition, 0.0, 1.0);
	passColor = vColor;
}