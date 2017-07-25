#include "poly_renderer.h"

#include <glad/glad.h>

#include <graphics/opengl/gl_api.h>
#include <graphics/pipeline.h>

void PolyRenderer::init() {
}

void PolyRenderer::terminate() {
	oak::graphics::shader::destroy(shader_);
}


void PolyRenderer::render(oak::graphics::Api *api) {


	//do rendering stuff
	glDisable(GL_DEPTH_TEST);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glEnable(GL_BLEND);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	api->draw(*pipeline_->batches[0]);

	oak::graphics::GLVertexArray::unbind();
}