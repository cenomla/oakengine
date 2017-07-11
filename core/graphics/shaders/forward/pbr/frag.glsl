#version 450 core

layout (binding = 0) uniform sampler2D u_sampler0;
layout (binding = 1) uniform sampler2D u_sampler1; //metallic
layout (binding = 2) uniform sampler2D u_sampler2; //roughness

in Pass {
	vec3 pos;
	vec3 normal;
	vec2 uv;
} frag;

layout (std140, binding = 4) uniform LightBlock {
	struct {
		vec4 pos;
		vec3 color;
	} data[8];
} u_lights;

const float c_PI = 3.14159265359;

layout (location = 0) out vec4 o_color;

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = c_PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {

	vec3 albedo = pow(texture(u_sampler0, frag.uv).rgb, vec3(2.2));
	float metallic = texture(u_sampler1, frag.uv).r;
	float roughness = texture(u_sampler2, frag.uv).r;

	vec3 N = normalize(frag.normal);
	vec3 V = normalize(-frag.pos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	//calculate radiance
	for (int i = 0; i < 8; i++) {
		vec3 toL = u_lights.data[i].pos.xyz - frag.pos;
		vec3 L = normalize(toL);
		vec3 H = normalize(L + V);
		float dist = length(toL);
		float att = u_lights.data[i].pos.w / (dist * dist);
		vec3 radiance = u_lights.data[i].color * att;

		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 nom = NDF * G * F;
		float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 spec = nom / denom;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / c_PI + spec) * radiance * NdotL;
	}

	//end radiance calculation
	vec3 ambient = vec3(0.01) * albedo;
	vec3 color = ambient + Lo;
	//tone map HDR to LDR
	color = color / (color + vec3(1.0));
	//gamma correct
	color = pow(color, vec3(1.0 / 2.2));


	o_color = vec4(color, 1.0);

}