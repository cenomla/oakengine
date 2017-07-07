#version 330 core

in vec2 passTexCoords;

uniform sampler2D tex;

layout (location = 0) out vec4 fragColor;

uniform float text_width = 0.5;
uniform float text_edge = 0.1;
uniform vec3 text_color = vec3(1.0, 1.0, 1.0);

uniform float border_width = 0.6;
uniform float border_edge = 0.2;
uniform vec3 border_color = vec3(0.0, 0.0, 0.0);

void main() {

	float dist = 1.0 - texture(tex, passTexCoords).a;
	float text_alpha = 1.0 - smoothstep(text_width, text_width + text_edge, dist);
	float border_alpha = 1.0 - smoothstep(border_width, border_width + border_edge, dist);

	float alpha = text_alpha + (1.0 - text_alpha) * border_alpha;
	vec3 color = mix(border_color, text_color, text_alpha / alpha);

	fragColor = vec4(color, alpha);
}