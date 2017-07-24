#version 450 core


in vec4 passColor;

layout (location = 0) out vec4 o_color;

void main() {
	o_color = passColor;
}