#pragma once

#include <glm/glm.hpp>
#include <graphics/renderable.h>
#include <pup.h>

struct LightComponent : public oak::graphics::Renderable {
	float size = 256.0f;
	glm::vec3 color{ 1.0f, 1.0f, 1.0f };
	size_t matId = std::hash<oak::string>{}("mat_light");

	void draw(void *buffer, float x, float y, float rotation, float scale) const override {
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

	size_t getMaterialId() const override {
		return matId;
	}

	size_t getVertexCount() const override {
		return 4;
	}
};

struct OccluderComponent {
	float height = 1.0f;
	float width = 1.0f;
	glm::vec2 hohoh{ 1.0f };
};


namespace oak {

	void pup(Puper& puper, LightComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, OccluderComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, ActionComponent& data, const ObjInfo& info);

}