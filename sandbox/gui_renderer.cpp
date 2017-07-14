#include "gui_renderer.h"

#include <glad/glad.h>

#include <graphics/opengl/gl_api.h>
#include <graphics/pipeline.h>
#include <graphics/material.h>

void GuiRenderer::init() {

}

void GuiRenderer::terminate() {

}


void GuiRenderer::render(oak::graphics::Api *api) {
	//do rendering stuff

	glDisable(GL_DEPTH_TEST);

	for (const auto& batch : *pipeline_->batches) {
		if (batch.layer != 1) { continue; }
		//bind material
		oak::graphics::shader::bind(*batch.material->shader);
		for (int i = 0; i < 16; i++) {
			if (batch.material->textures[i] != nullptr) {
				oak::graphics::texture::bind(*batch.material->textures[i], i);
			}
		}
		batch.storage->bind();
		//render stuff

		glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));
	}

	oak::graphics::GLVertexArray::unbind();

}