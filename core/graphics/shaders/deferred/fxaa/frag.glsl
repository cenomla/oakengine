#version 450 core

uniform sampler2D tex0; // 0

uniform float width = 1280.0;
uniform float height = 720.0;
uniform float spanMax = 8.0;
uniform float reduceMul = 1.0 / 8.0;
uniform float reduceMin = 1.0 / 128.0;

const float gamma = 2.2;

in vec4 passUV;

layout (location = 0) out vec4 outColor;

vec3 fxaa(vec4 uv, sampler2D tex, vec2 fs) {

	vec3 rgbNW = texture(tex, uv.zw).xyz;
	vec3 rgbNE = texture(tex, uv.zw + vec2(1.0, 0.0) * fs).xyz;
	vec3 rgbSW = texture(tex, uv.zw + vec2(0.0, 1.0) * fs).xyz;
	vec3 rgbSE = texture(tex, uv.zw + vec2(1.0) * fs).xyz;
	vec3 rgbM  = texture(tex, uv.xy).xyz;
	

	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	


	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	

	vec2 dir; 
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	

	float dirReduce = max(
		(lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * reduceMul),
		reduceMin);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2( spanMax,  spanMax), 
		  max(vec2(-spanMax, -spanMax), 
		  dir * rcpDirMin)) * fs;
	

	vec3 rgbA = (1.0/2.0) * (
		texture(tex, uv.xy + dir * (1.0/3.0 - 0.5)).xyz +
		texture(tex, uv.xy + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		texture(tex, uv.xy + dir * (0.0/3.0 - 0.5)).xyz +
		texture(tex, uv.xy + dir * (3.0/3.0 - 0.5)).xyz);

	float lumaB = dot(rgbB, luma);

	if ((lumaB < lumaMin) || (lumaB > lumaMax)) { 
		return rgbA;
	}

	return rgbB; 
}
	
void main() {
	vec2 fs = vec2(1.0 / width, 1.0 / height);
	vec3 clinear = fxaa(passUV, tex0, fs);
	outColor = vec4(pow(clinear, vec3(1.0 / gamma)), 1.0);
}