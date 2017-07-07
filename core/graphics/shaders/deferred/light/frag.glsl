#version 450 core

uniform sampler2D textures[3];

layout (shared) uniform MatrixBlock {
	mat4 proj;
	mat4 view;
} invMatrix;

in vec2 passUV;

layout (location = 0) out vec4 outColor;

void main() {

	vec4 albedo = texture(textures[0], passUV);
	vec3 normal = texture(textures[1], passUV).xyz;
	float depth = texture(textures[2], passUV).x;

    const float zNear = 0.5;
	const float zFar = 200.0;

	float z_n = 2.0 * depth - 1.0;
	float z = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

	vec4 nds = vec4(passUV * 2.0 - 1.0, z_n, 1.0);
	vec4 pvs = invMatrix.proj * nds;
	vec3 vs = pvs.xyz / pvs.w;
	vec4 pws = invMatrix.view * vec4(vs, 1.0);
	vec3 ws = pws.xyz;

	vec3 toLight = normalize(vec3(14.0, 16.0, 12.0) - ws);
	float level = max(dot(normalize(normal), toLight), 0.0);

	outColor = vec4(mix(albedo.rgb * max(level, 0.1), vec3(0.3, 0.5, 0.9), z / zFar), 1.0);

}