#version 450 core

uniform sampler2D texNormal;
uniform sampler2D texDepth;
uniform sampler2D texNoise;

layout (std140) uniform MatrixBlock {
	mat4 invProj;
	mat4 proj;
} matrix;

layout (std140) uniform KernelBlock {
	vec4 samples[128];
	float radius;
	int count;
} kernel;

const float zNear = 0.5;
const float zFar = 500.0;

const vec2 nscale = vec2(1280.0 / 4.0, 720.0 / 4.0);

in vec2 passUV;

layout (location = 0) out float outAo;

float ssao(vec3 pos, vec3 normal, vec2 uv) {
	//sample noise
	vec3 noise = texture(texNoise, uv * nscale).xyz;

	vec3 tangent = normalize(noise - normal * dot(noise, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	const float bias = 0.025;

	float occlusion = 0.0;
	for (int i = 0; i < kernel.count; i++) {
		vec3 s = tbn * kernel.samples[i].xyz;
		s = pos + s * kernel.radius;

		vec4 offset = vec4(s, 1.0);
		offset = matrix.proj * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sd = texture(texDepth, offset.xy).x;
		float lsd = zNear * zFar / ((sd * (zFar - zNear)) - zFar);
		float rc = smoothstep(0.0, 1.0, kernel.radius / abs(pos.z - lsd));
		occlusion += (lsd >= s.z + bias ? 1.0 : 0.0) * rc;
	}

	occlusion = 1.0 - (occlusion / float(kernel.count));

	return pow(occlusion, 1.4);
}

void main() {

	vec3 normal = texture(texNormal, passUV).xyz;
	float depth = texture(texDepth, passUV).x;

	//reconstruct position from depth
	vec4 nds = vec4(passUV * 2.0 - 1.0, 2.0 * depth - 1.0, 1.0);
	vec4 vs = matrix.invProj * nds;
	vs /= vs.w;

	float ao = ssao(vs.xyz, normal, passUV);

	outAo = ao;

}