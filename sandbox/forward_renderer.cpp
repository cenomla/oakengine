#include "forward_renderer.h"

#include <glad/glad.h>
#include <graphics/material.h>
#include <graphics/pipeline.h>

void ForwardRenderer::init() {

}

void ForwardRenderer::terminate() {
}

void ForwardRenderer::render(oak::graphics::Api *api) {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//do rendering stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& batch : *pipeline_->batches) {
		if (batch.layer != 0) { continue; }
		//bind material 
		glUseProgram(batch.material->shader->id);
		for (int i = 0; i < 16; i++) {
			if (batch.material->textures[i] != nullptr) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, batch.material->textures[i]->id);
			}
		}
		batch.storage->bind();
		//render stuff

		glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));
	}

}