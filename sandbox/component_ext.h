#pragma once

#include <glm/glm.hpp>
#include <pup.h>

struct LightComponent {

	LightComponent() = default;
	LightComponent(float s, const glm::vec3& c, size_t mi) : size{ s }, color{ c }, matId{ mi } {}

	float size = 256.0f;
	glm::vec3 color{ 1.0f, 1.0f, 1.0f };
	size_t matId = std::hash<oak::string>{}("mat_light");

	void draw(void *buffer, float x, float y, float rotation, float scale) const {
		struct LV {
			glm::vec2 position;
			glm::vec2 texCoords;
		};

		LV* data = static_cast<LV*>(buffer);
		data[0] = { glm::vec2{ x - size, y - size }, glm::vec2{ 0.0f, 0.0f } };
		data[1] = { glm::vec2{ x + size, y - size }, glm::vec2{ 1.0f, 0.0f } };
		data[2] = { glm::vec2{ x + size, y + size }, glm::vec2{ 1.0f, 1.0f } };
		data[3] = { glm::vec2{ x - size, y + size }, glm::vec2{ 0.0f, 1.0f } };
	}
};

struct OccluderComponent {
	float height = 1.0f;
};


namespace oak {

	void pup(Puper& puper, LightComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, OccluderComponent& comp, const ObjInfo& info);

}