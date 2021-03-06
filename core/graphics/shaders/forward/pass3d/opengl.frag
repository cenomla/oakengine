#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUVs;

uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {

	vec3 toLight = normalize(vec3(140.0, 160.0, 120.0) - fragPos);
	float level = max(dot(normalize(fragNormal), toLight), 0.0);

	vec4 texel = texture(texSampler, fragUVs);
	outColor = vec4(texel.rgb * max(level, 0.2), texel.a);
}