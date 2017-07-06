#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

layout (shared) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec3 passNormal = vNormal;
out vec2 passUV = vUV;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {

	gl_Position = matrix.proj * matrix.view * vec4(vPosition, 1.0);

	passNormal = normalize(vNormal);
	passUV = vUV;

}