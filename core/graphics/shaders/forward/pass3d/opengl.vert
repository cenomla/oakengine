#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUVs;

layout (shared) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 fragUVs;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = matrix.proj * matrix.view * vec4(vPosition, 1.0);
	fragNormal = vNormal;
	fragPos = vPosition;
	fragUVs = vUVs;
}
