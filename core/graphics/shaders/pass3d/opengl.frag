#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUVs;

uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {

	vec3 toLight = normalize(vec3(14.0, 16.0, 12.0) - fragPos);
	float level = max(dot(fragNormal, toLight), 0.0);

	vec4 texel = texture(texSampler, fragUVs);
	outColor = mix(vec4(texel.rgb * max(level, 0.2), texel.a), vec4((fragNormal + 1) * 0.5, 1.0), 0.2);
}