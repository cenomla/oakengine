#version 450 core

layout (location = 1) in vec2 vPosition;

out vec2 passUV;
 
void main() {
	gl_Position = vec4(vPosition, 0.0, 1.0);
	passUV = vPosition * 0.5 + 0.5;
}