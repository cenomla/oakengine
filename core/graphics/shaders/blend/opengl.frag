#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUVs;

uniform sampler2D texSampler[4];

layout (location = 0) out vec4 outColor;

void main() {

	vec3 toLight = normalize(vec3(140.0, 160.0, 120.0) - fragPos);
	float level = max(dot(fragNormal, toLight), 0.0);

	vec4 blend = texture(texSampler[0], fragUVs);
	vec4 tex1 = texture(texSampler[1], fragUVs * 4);
	vec4 tex2 = texture(texSampler[2], fragUVs * 4);
	vec4 tex3 = texture(texSampler[3], fragUVs * 4);

	vec4 color = vec4((tex1.rgb * blend.r) + (tex2.rgb * blend.g) + (tex3.rgb * blend.b), 1.0);

	outColor = vec4(color.rgb * max(level, 0.2), color.a);
}