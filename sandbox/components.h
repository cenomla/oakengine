#pragma once

#include <glm/glm.hpp>
#include <graphics/model.h>
#include <graphics/material.h>
#include <resource.h>
#include <pup.h>

struct TransformComponent {
	glm::mat4 transform;
};

struct CameraComponent {
	glm::vec3 position;
	glm::vec3 rotation;
};

struct ModelComponent {
	oak::Resource<oak::graphics::Model> model{ 0 };
	oak::Resource<oak::graphics::Material> material{ 0 };
	oak::graphics::TextureRegion region;
	uint32_t layer = 0;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, CameraComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, ModelComponent& data, const oak::ObjInfo& info);