#version 330 core

uniform mat4 proj;
uniform mat4 view;

layout (location = 0) in vec3 vPosition;


void main() {

	gl_Position = proj * view * vec4(vPosition, 1.0);

}