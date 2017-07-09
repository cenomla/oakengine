#version 450 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;

layout (std140, binding = 1) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out Pass {
	vec2 uv;
} pass;

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(v_pos, 0.0, 1.0);
	pass.uv = v_uv;
}

