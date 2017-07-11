#version 450 core

layout (binding = 0) uniform sampler2D texAlbedo; //albedo roughness
layout (binding = 1) uniform sampler2D texNormal; //normal metalness
layout (binding = 2) uniform sampler2D texDepth;
layout (binding = 3) uniform sampler2D texAo;

layout (std140, binding = 2) uniform MatrixBlock {
	mat4 invProj;
	mat4 proj;
} u_matrix;

layout (std140, binding = 4) uniform LightBlock {
	struct {
		vec4 pos;
		vec3 color;
	} data[8];
} u_lights;

uniform vec3 u_ambientColor = vec3(0.01);

in vec2 passUV;

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

float blurAo() {
	vec2 ts = 1.0 / vec2(textureSize(texAo, 0));
	float r = 0.0;
	for (int x = -2; x < 2; x++) {
		for (int y = -2; y < 2; y++) {
			vec2 o = vec2(float(x), float(y)) * ts;
			r += texture(texAo, passUV + o).x;
		}
	}
	return r / 16.0;
}

void main() {

	vec4 s_ar = texture(texAlbedo, passUV);
	vec4 s_nm = texture(texNormal, passUV);
	vec3 albedo = s_ar.rgb;
	float roughness = s_ar.w;
	vec3 normal = s_nm.xyz * 2.0 - 1.0;
	float metalness = s_nm.w;
	float depth = texture(texDepth, passUV).x;

	//reconstruct position from depth
	vec4 nds = vec4(passUV * 2.0 - 1.0, 2.0 * depth - 1.0, 1.0);
	vec4 vs = u_matrix.invProj * nds;
	vs /= vs.w;

	vec3 N = normalize(normal);
	vec3 V = normalize(-vs.xyz);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metalness);

	float ao = blurAo();

	vec3 Lo = vec3(0.0);
	//calculate radiance
	for (int i = 0; i < 8; i++) {
		vec3 toL = u_lights.data[i].pos.xyz - vs.xyz;
		vec3 L = normalize(toL);
		vec3 H = normalize(L + V);
		float dist = length(toL);
		float att = u_lights.data[i].pos.w / (dist * dist);
		vec3 radiance = u_lights.data[i].color * att;

		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 dif = vec3(1.0) - F;
		dif *= 1.0 - metalness;

		vec3 nom = NDF * G * F;
		float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 spec = nom / denom;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (dif * albedo / c_PI + spec) * radiance * NdotL * ao;
	}

	//end radiance calculation
	vec3 ambient = u_ambientColor * albedo * ao;
	vec3 color = (ambient + Lo);
	//tone map HDR to LDR
	color = color / (color + vec3(1.0));
	//gamma correct
	color = pow(color, vec3(1.0 / 2.2));


	o_color = vec4(color, 1.0);
}