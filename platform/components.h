#pragma once

#include <glm/glm.hpp>
#include <graphics/material.h>
#include <graphics/sprite.h>
#include <graphics/font.h>
#include <collision.h>
#include <resource.h>

namespace oak {
	class Puper;
	struct ObjInfo;
}

struct TransformComponent {
	glm::mat3 transform;
};

struct VelocityComponent {
	glm::vec2 velocity;
	glm::vec2 acceleration;
};

struct MeshComponent {
	oak::Resource<oak::Mesh2d> mesh;
};

struct RigidBodyComponent {
	float restitution = 0.2f;
	float mass = 0.0f;
	float invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
};

struct SpriteComponent {
	oak::Resource<oak::graphics::Sprite> sprite;
	oak::Resource<oak::graphics::Material> material;
	uint32_t layer = 0;
};

struct TextComponent {
	oak::string text;
	oak::Resource<oak::graphics::Font> font;
	oak::Resource<oak::graphics::Material> material;
	uint32_t layer = 0;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, RigidBodyComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, SpriteComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, TextComponent& data, const oak::ObjInfo& info);
