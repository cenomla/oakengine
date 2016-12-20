#version 330 core

in vec2 passTexCoords;

uniform sampler2D tex;

layout (location = 0) out vec4 fragColor;

const float width = 0.5;
const float edge = 0.1;


void main() {

	float dist = 1.0 - texture(tex, passTexCoords).a;
	float alpha = 1.0 - smoothstep(width, width + edge, dist);

	fragColor = vec4(1.0, 1.0, 1.0, alpha);
}