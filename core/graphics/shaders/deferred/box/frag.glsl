#version 450 core

layout (binding = 0) uniform samplerCube skybox;

in vec3 passUVR;

layout (location = 0) out vec4 albedo;

void main() {

	albedo = texture(skybox, passUVR);

}