#version 450 core

layout (location = 1) in vec2 vPos;
layout (location = 3) in vec2 vUV;

layout (std140, binding = 1) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out Pass {
	vec2 uv;
} pass;

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(vPos, 0.0, 1.0);
	pass.uv = vUV;
}

