#version 450 core

layout (location = 0) in vec2 vPosition;

uniform float subpix = 1.0 / 4.0;
uniform float width = 1280.0;
uniform float height = 720.0;

out vec4 passUV;

void main() {
	gl_Position = vec4(vPosition, 0.0, 1.0);

	vec2 uv = vPosition * 0.5 + 0.5;
	vec2 fs = vec2(1.0 / width, 1.0 / height);  

	passUV.xy = uv;
	passUV.zw = uv - (fs * (0.5 + subpix));
}