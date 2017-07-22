#pragma once

#include <glm/glm.hpp>
#include <graphics/material.h>
#include <graphics/sprite.h>
#include <mesh.h>
#include <pup.h>

struct TransformComponent {
	glm::mat4 transform;
};

struct Transform2dComponent {
	glm::mat3 transform;
};

struct CameraComponent {
	glm::vec3 position;
	glm::vec3 rotation;
};

struct MeshComponent {
	const oak::graphics::Mesh *mesh = nullptr;
	const oak::graphics::Material *material = nullptr;
	oak::graphics::TextureRegion region;
	uint32_t layer = 0;
};

struct SpriteComponent {
	const oak::graphics::Sprite *sprite = nullptr;
	const oak::graphics::Material *material = nullptr;
	uint32_t layer = 0;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, Transform2dComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, CameraComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, SpriteComponent& data, const oak::ObjInfo& info);