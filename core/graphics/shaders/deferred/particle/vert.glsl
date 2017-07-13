#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vUV;
layout (location = 6) in vec3 vOffset;

layout (std140, binding = 0) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec3 passNormal;
out vec2 passUV;

void main() {

	gl_Position = matrix.proj * matrix.view * vec4(vPosition + vOffset, 1.0);

	passNormal = (matrix.view * vec4(vNormal, 0.0)).xyz;
	passUV = vUV;

}