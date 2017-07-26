#include "sprite_renderer.h"

#include <glad/glad.h>

#include <graphics/gl_api.h>
#include <graphics/pipeline.h>
#include <graphics/material.h>

void SpriteRenderer::init() {

}

void SpriteRenderer::terminate() {

}


void SpriteRenderer::render(oak::graphics::Api *api) {


	//do rendering stuff
	glDisable(GL_DEPTH_TEST);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	api->draw(*pipeline_->batches[1]);

	oak::graphics::GLVertexArray::unbind();

	glDisable(GL_BLEND);
}