#version 450 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

layout (std140) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out Pass {
	vec3 normal;
	vec3 pos,
	vec2 uv
} pass;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(v_pos, 1.0);
	pass.pos = v_pos;
	pass.normal = v_normal;
	pass.uv = v_uv;
}
