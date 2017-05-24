#version 330 core

in vec2 fragUVs;

uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, fragUVs);
}