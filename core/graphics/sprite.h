#pragma once

#include <glm/glm.hpp>
#include "texture.h"

namespace oak::graphics {


	class Sprite {
	public:
		struct Vertex {
			glm::vec2 position;
			glm::vec2 uv;
		};

		Sprite(float centerX, float centerY, float width, float height, float dx, float dy, float dw, float dh);
		Sprite(float centerX, float centerY, float width, float height, const TextureRegion &region, int maxFramesX = 1, int maxFramesY = 1);

		void draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float rotation, float scaleX, float scaleY) const;
	private:
		float centerX_, centerY_, width_, height_, dx_, dy_, dw_, dh_;
	};

}