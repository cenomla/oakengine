#version 450 core

uniform sampler2D texDiffuse;
uniform sampler2D texSpec;
uniform sampler2D texEmissive;

in vec3 passNormal;
in vec2 passUV;

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;

void main() {

	albedo = vec4(texture(texDiffuse, passUV).rgb, texture(texSpec, passUV).r);
	normal = vec4(normalize(passNormal), texture(texEmissive, passUV).r);

}