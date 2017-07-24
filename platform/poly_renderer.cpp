#include "poly_renderer.h"

#include <glad/glad.h>

#include <graphics/opengl/gl_api.h>
#include <graphics/pipeline.h>

void PolyRenderer::init() {
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "/res/shaders/poly_shader/vert.glsl";
	shaderInfo.fragment = "/res/shaders/poly_shader/frag.glsl";
	shader_ = oak::graphics::shader::create(shaderInfo);
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


	for (const auto& batch : *pipeline_->batches) {
		//bind material
		oak::graphics::shader::bind(shader_);
		batch.storage->bind();
		//render stuff

		glDrawArrays(GL_LINE_LOOP, batch.offset, batch.count);
	}

	oak::graphics::GLVertexArray::unbind();
}