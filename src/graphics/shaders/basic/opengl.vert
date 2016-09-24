#version 330 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;

out vec3 fragColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = vec4(vPosition, 0.0, 1.0);
	fragColor = vColor;
}
