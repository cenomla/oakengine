#version 330 core

layout (location = 0) in vec2 vPosition;

out vec2 texCoords;

void main() {
	gl_Position = vec4(vPosition, 0.0, 1.0);
	gl_Position = sign(gl_Position);

	texCoords = (vPosition + vec2(1.0)) / 2.0;
}