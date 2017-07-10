#version 450 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

layout (std140, binding = 0) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out Pass {
	vec3 pos;
	vec3 normal;
	vec2 uv;
} pass;

void main() {

	vec4 viewPos = matrix.view * vec4(v_pos, 1.0);
	gl_Position = matrix.proj * viewPos;

	pass.pos = viewPos.xyz;
	pass.normal = (matrix.view * vec4(v_normal, 0.0)).xyz;
	pass.uv = v_uv;

}