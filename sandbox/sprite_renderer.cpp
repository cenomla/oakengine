#include "sprite_renderer.h"

#include <graphics/gl_api.h>
#include <graphics/material.h>

#include "pipeline.h"

void SpriteRenderer::init() {

}

void SpriteRenderer::terminate() {

}


void SpriteRenderer::render(oak::graphics::Api *api) {

	static oak::graphics::Api::State state;
	state.viewport = oak::graphics::View{ pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height };
	state.blendMode = oak::graphics::BlendMode::NORMAL;

	api->setState(state);

	for (const auto& batch : *pipeline_->batches[1]) {
		api->draw(batch);
	}

	oak::graphics::GLVertexArray::unbind();
}