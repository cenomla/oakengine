#version 330 core

in vec2 passTexCoords;

uniform sampler2D tex;

layout (location = 0) out vec4 fragColor;

void main() {
	fragColor = texture(tex, passTexCoords);
}