#version 450 core

uniform sampler2D textures[3];

layout (std140) uniform MatrixBlock {
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
	const float zFar = 500.0;

	float z_n = 2.0 * depth - 1.0;
	float z = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

	vec4 nds = vec4(passUV * 2.0 - 1.0, z_n, 1.0);
	vec4 vs = invMatrix.proj * nds;
	vs /= vs.w;
	vec4 ws = invMatrix.view * vs;

	vec3 toLight = normalize(vec3(14.0, 16.0, 12.0) - ws.xyz);
	float level = max(dot(normal, toLight), 0.0);

	outColor = vec4(mix(albedo.rgb * max(level, 0.1), vec3(0.3, 0.5, 0.9), z / zFar), albedo.a);
}