#version 330 core

in vec2 texCoords;

uniform sampler2D tex;

layout (location = 0) out vec4 fragColor;

void main() {
	vec3 t = texture(tex, texCoords).xyz;
	t *= 2.0;
	vec3 mapped = vec3(1.0) - exp(-t * 0.1);
	mapped = pow(mapped, vec3(1.0 / 2.2));
	fragColor = vec4(mapped, 1.0);
}