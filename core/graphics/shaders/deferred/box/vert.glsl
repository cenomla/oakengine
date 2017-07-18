#version 450 core

layout (location = 0) in vec3 vPosition;

layout (std140, binding = 0) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} matrix;

out vec3 passUVR;

void main() {
	vec4 pos = matrix.proj * mat4(mat3(matrix.view)) * vec4(vPosition, 1.0);
	gl_Position = pos.xyww;
	passUVR = vPosition;
}