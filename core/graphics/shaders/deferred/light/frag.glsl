#version 450 core

uniform sampler2D texAlbedo;
uniform sampler2D texNormal;
uniform sampler2D texDepth;
uniform sampler2D texAo;

layout (std140) uniform MatrixBlock {
	mat4 invProj;
	mat4 proj;
} matrix;

const vec3 lightPos = vec3(16.0, 4.0, 32.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightRadius = 128.0;

const vec3 ambientColor = vec3(0.1, 0.1, 0.1);

in vec2 passUV;

layout (location = 0) out vec4 outColor;

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

vec3 light(vec3 lpos, vec3 lcolor, float lrad, vec3 pos, vec3 n, vec3 dcolor, float dspec) {
	//calculate lighting contribution
	vec3 l = - pos;
	vec3 v = normalize(pos);
	vec3 h = normalize(l + v);

	float att = clamp(1.0 - length(l) / lrad, 0.0, 1.0);
	if (att < 0.00001) { return vec3(0.0); }
	l = normalize(l);
	

	vec3 lamb = clamp(dot(n, l), 0.0, 1.0) * dcolor * lcolor;
	vec3 spec = pow(clamp(dot(n, h), 0.0, 1.0), dspec) * dcolor * lcolor;

	return att * (lamb + spec);
}

void main() {

	vec4 albedo = texture(texAlbedo, passUV);
	vec4 normEm = texture(texNormal, passUV);
	vec3 normal = normEm.xyz;
	float em = normEm.w;
	float depth = texture(texDepth, passUV).x;

	//reconstruct position from depth
	vec4 nds = vec4(passUV * 2.0 - 1.0, 2.0 * depth - 1.0, 1.0);
	vec4 vs = matrix.invProj * nds;
	vs /= vs.w;

	vec3 l;
	float ao = blurAo();
	if (em < 0.0001) {
		l = light(lightPos, lightColor, lightRadius, vs.xyz, normal, albedo.rgb, albedo.a * 256.0);
	} else {
		l = albedo.rgb * em;
	}

	outColor = vec4(max(l * ao, ambientColor), 1.0);
}