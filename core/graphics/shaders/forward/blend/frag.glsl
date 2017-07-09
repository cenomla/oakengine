#version 450 core

layout (binding = 0) uniform sampler2D u_sampler0;
layout (binding = 1) uniform sampler2D u_sampler1;
layout (binding = 2) uniform sampler2D u_sampler2;
layout (binding = 3) uniform sampler2D u_sampler3;

in Pass {
	vec3 normal;
	vec3 pos;
	vec3 uv;
} frag;

layout (location = 0) out vec4 o_color;

void main() {

	vec3 toLight = normalize(vec3(140.0, 160.0, 120.0) - frag.pos);
	float level = max(dot(normalize(fragNormal), toLight), 0.0);

	vec4 blend = texture(u_sampler0, frag.uv);
	vec4 tex1 = texture(u_sampler1, frag.uv * 4);
	vec4 tex2 = texture(u_sampler2, frag.uv * 4);
	vec4 tex3 = texture(u_sampler3, frag.uv * 4);

	vec4 color = vec4((tex1.rgb * blend.r) + (tex2.rgb * blend.g) + (tex3.rgb * blend.b), 1.0);

	o_color = vec4(color.rgb * max(level, 0.2), color.a);
}