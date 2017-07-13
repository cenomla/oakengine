#version 450 core

layout (binding = 0) uniform sampler2D texAlbedo;
layout (binding = 1) uniform sampler2D texRoughness;
layout (binding = 2) uniform sampler2D texMetalness;

in vec3 passNormal;
in vec2 passUV;

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;

void main() {

	albedo = vec4(texture(texAlbedo, passUV).rgb, texture(texRoughness, passUV).r);
	normal = vec4(normalize(passNormal) * 0.5 + 0.5, texture(texMetalness, passUV).r);

}