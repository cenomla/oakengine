#include "sprite.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace oak::graphics {

	Sprite::Sprite(float centerX, float centerY, float width, float height, float dx, float dy, float dw, float dh) : 
		centerX_{ centerX }, centerY_{ centerY }, width_{ width }, height_{ height }, dx_{ dx }, dy_{ dy }, dw_{ dw }, dh_{ dh } {}

	Sprite::Sprite(float centerX, float centerY, float width, float height, const TextureRegion &region, int maxFramesX, int maxFramesY) :
		Sprite{ centerX, centerY, width, height, region.pos.x, region.pos.y, region.extent.x / maxFramesX, region.extent.y / maxFramesY } {}

	void Sprite::draw(void *buffer, float x, float y, int animFrameX, int animFrameY, float rotation, float sx, float sy) const {
		float u = dx_ + animFrameX * dw_;
		float v = dy_ + animFrameY * dh_;
		glm::vec2 pos{ x, y };
		static_cast<Vertex*>(buffer)[0] = { glm::rotate(glm::vec2{ -centerX_, -centerY_ } * glm::vec2{ sx, sy }, rotation) + pos, glm::vec2{ u, v } };
		static_cast<Vertex*>(buffer)[1] = { glm::rotate(glm::vec2{ -centerX_ + width_, -centerY_ } * glm::vec2{ sx, sy }, rotation) + pos, glm::vec2{ u + dw_, v } };
		static_cast<Vertex*>(buffer)[2] = { glm::rotate(glm::vec2{ -centerX_ + width_, -centerY_ + height_ } * glm::vec2{ sx, sy }, rotation) + pos, glm::vec2{ u + dw_, v + dh_ } };
		static_cast<Vertex*>(buffer)[3] = { glm::rotate(glm::vec2{ -centerX_, -centerY_ + height_ } * glm::vec2{ sx, sy }, rotation) + pos, glm::vec2{ u, v + dh_ } };
	}

}