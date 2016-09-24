#version 330 core

in vec3 fragColor;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = vec4(fragColor, 1.0);
}