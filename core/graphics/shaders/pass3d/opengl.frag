#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUVs;

uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {

	float level = 0.0;
	vec3 toLight = normalize(vec3(4.0, 8.0, 10.0));


	float p = dot(toLight, normalize(fragNormal));
	if (p > 0.0) {
		level = p;
	}

	vec4 texel = texture(texSampler, fragUVs);
	outColor = vec4(texel.rgb * max(p, 0.4), texel.a);
}