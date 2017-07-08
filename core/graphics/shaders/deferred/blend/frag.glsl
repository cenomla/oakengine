#version 450 core

uniform sampler2D textures[7];

in vec3 passNormal;
in vec2 passUV;

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;

void main() {

	vec4 tex0 = texture(textures[0], passUV);
	vec4 tex1 = texture(textures[1], passUV * 4.0);
	vec4 tex2 = texture(textures[2], passUV * 4.0);
	vec4 tex3 = texture(textures[3], passUV * 4.0);
	
	vec4 spec1 = texture(textures[4], passUV * 4.0);
	vec4 spec2 = texture(textures[5], passUV * 4.0);
	vec4 spec3 = texture(textures[6], passUV * 4.0);

	albedo = tex1 * tex0.r + tex2 * tex0.g + tex3 * tex0.b;
	albedo.a = spec1.r * tex0.r + spec2.r * tex0.g + spec3.r * tex0.b;
	albedo.a = clamp(albedo.a, 0.0, 1.0);

	normal = vec4(normalize(passNormal), 0.0);

}