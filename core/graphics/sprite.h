#pragma once

#include <glm/glm.hpp>

#include "texture_region.h"

namespace oak::graphics {


	class Sprite {
	public:
		struct Vertex {
			Vertex(const glm::vec2 &pos, const glm::vec2 &tex) : position{ pos }, texCoords{ tex } {}
			Vertex(float x, float y, float u, float v) : position{ x, y }, texCoords{ u, v } {}

			glm::vec2 position;
			glm::vec2 texCoords;
		};
		
		Sprite(size_t materialId, float centerX, float centerY, float width, float height, float dx, float dy, float dw, float dh);
		Sprite(size_t materialId, float centerX, float centerY, float width, float height, const TextureRegion &region, int maxFramesX = 1, int maxFramesY = 1);

		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY) const;
		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float rotation, float scale) const;

		size_t getMaterialId() const { return materialId_; }
	private:
		size_t materialId_;
		float centerX_, centerY_, width_, height_, dx_, dy_, dw_, dh_;
	};

}