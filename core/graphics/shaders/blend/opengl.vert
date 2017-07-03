#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUVs;

layout (shared) uniform MatrixBlock {
	mat4 model;
	mat4 view;
	mat4 proj;
} matrix;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 fragUVs;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	vec4 pos = matrix.model * vec4(vPosition, 1.0);
	gl_Position = matrix.proj * matrix.view * pos;
	fragNormal = normalize((matrix.model * vec4(vNormal, 0.0)).xyz);
	fragPos = pos.xyz;
	fragUVs = vUVs;
}
