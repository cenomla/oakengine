#version 450 core

layout (binding = 0) uniform sampler2D u_sampler0;

in Pass {
	vec2 uv;
} frag;

layout (location = 0) out vec4 o_color;

void main() {
	o_color = texture(u_sampler0, frag.uv);
}