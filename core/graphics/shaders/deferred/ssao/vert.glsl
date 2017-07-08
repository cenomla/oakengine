#version 450 core

layout (location = 0) in vec2 vPosition;

out vec2 passUV;

void main() {

	gl_Position = vec4(vPosition, 0.0, 1.0);

	passUV = (vPosition + vec2(1.0)) / 2.0;

}
