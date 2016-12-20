#include "sprite.h"

#include <glm/gtx/rotate_vector.hpp>

namespace oak::graphics {

	Sprite::Sprite(size_t materialId, float width, float height, float dx, float dy, float dw, float dh, float centerX, float centerY) : 
		materialId_{ materialId }, width_{ width }, height_{ height }, dx_{ dx }, dy_{ dy }, dw_{ dw }, dh_{ dh }, centerX_{ centerX }, centerY_{ centerY } {

	}

	Sprite::Sprite(size_t materialId, float width, float height, const TextureRegion &region, float centerX, float centerY) :
		Sprite{ materialId, width, height, region.dx, region.dy, region.dw, region.dh, centerX, centerY } {};

	void Sprite::draw(void *buffer, float x, float y, int animFrameX, int animFrameY) const {
		float u = dx_ + animFrameX * dw_;
		float v = dy_ + animFrameY * dh_;
		x -= centerX_;
		y -= centerY_;
		static_cast<Vertex*>(buffer)[0] = {x, y, u, v};
		static_cast<Vertex*>(buffer)[1] = {x + width_, y, u + dw_, v};
		static_cast<Vertex*>(buffer)[2] = {x + width_, y + height_, u + dw_, v + dh_};
		static_cast<Vertex*>(buffer)[3] = {x, y + height_, u, v + dh_};
	}

	void Sprite::draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float rotation, float scale) const {
		float u = dx_ + animFrameX * dw_;
		float v = dy_ + animFrameY * dh_;
		glm::vec2 pos{ x, y };
		static_cast<Vertex*>(buffer)[0] = { glm::rotate(glm::vec2{ -centerX_, -centerY_ } * scale, rotation) + pos, glm::vec2{ u, v } };
		static_cast<Vertex*>(buffer)[1] = { glm::rotate(glm::vec2{ -centerX_ + width_, -centerY_ } * scale, rotation) + pos, glm::vec2{ u + dw_, v } };
		static_cast<Vertex*>(buffer)[2] = { glm::rotate(glm::vec2{ -centerX_ + width_, -centerY_ + height_ } * scale, rotation) + pos, glm::vec2{ u + dw_, v + dh_ } };
		static_cast<Vertex*>(buffer)[3] = { glm::rotate(glm::vec2{ -centerX_, -centerY_ + height_ } * scale, rotation) + pos, glm::vec2{ u, v + dh_ } };
	}

}