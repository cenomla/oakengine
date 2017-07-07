#version 450 core

uniform sampler2D tex;

in vec3 passNormal;
in vec2 passUV;

layout (location = 0) out vec4 albeido;
layout (location = 1) out vec3 normal;

void main() {

	albeido = texture(tex, passUV);
	normal = normalize(passNormal);

}