#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, fragTexCoord * 4.0) * vec4(fragColor, 1.0);
}