#pragma once

#include <glm/glm.hpp>

#include "graphics/material.h"

namespace oak::graphics {


	class Sprite {
	public:
		struct Vertex {
			Vertex(const glm::vec2 &pos, const glm::vec2 &tex) : position{ pos }, texCoords{ tex } {}
			Vertex(float x, float y, float u, float v) : position{ x, y }, texCoords{ u, v } {}

			glm::vec2 position;
			glm::vec2 texCoords;
		};
		Sprite(MaterialId materialId, float width, float height, float dx, float dy, float dw, float dh, float centerX, float centerY);

		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY) const;
		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float scale, float rotation) const;

		MaterialId getMaterialId() const { return materialId_; }
	private:
		MaterialId materialId_;
		float width_, height_, dx_, dy_, dw_, dh_, centerX_, centerY_;
	};

}