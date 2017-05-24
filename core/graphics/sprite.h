#pragma once

#include <cstddef>
#include <glm/glm.hpp>

#include "texture_region.h"

namespace oak::graphics {


	class Sprite {
	public:
		Sprite(size_t materialId, float centerX, float centerY, float width, float height, float dx, float dy, float dw, float dh);
		Sprite(size_t materialId, float centerX, float centerY, float width, float height, const TextureRegion &region, int maxFramesX = 1, int maxFramesY = 1);

		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float rotation, float scaleX, float scaleY) const;

		size_t getMaterialId() const { return materialId_; }
	private:
		size_t materialId_;
		float centerX_, centerY_, width_, height_, dx_, dy_, dw_, dh_;
	};

}