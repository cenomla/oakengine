#include "deferred_renderer.h"

#include <glad/glad.h>
#include <graphics/material.h>
#include <graphics/api.h>
#include <graphics/pipeline.h>
#include <graphics/opengl/gl_texture.h>

#include <resource_manager.h>

void DeferredRenderer::init() {
	light_.create("core/graphics/shaders/deferred/light/vert.glsl", "core/graphics/shaders/deferred/light/frag.glsl");

	light_.bind();
	light_.setUniform("textures[0]", 0);
	light_.setUniform("textures[1]", 1);
	light_.setUniform("textures[2]", 2);
	light_.unbind();

	light_.bindBlockIndex("MatrixBlock", 2);
	
	oak::graphics::AttributeLayout layout{ oak::vector<oak::graphics::AttributeType>{ 
		oak::graphics::AttributeType::POSITION2D
	} };
	buffer_.create(&layout);

	float vdata[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f, 
		1.0f, 1.0f,
		-1.0f, 1.0f
	};

	unsigned int edata[] = {
		0, 1, 2, 2, 3, 0
	};

	buffer_.data(0, sizeof(vdata), vdata, GL_STATIC_DRAW);
	buffer_.data(1, sizeof(edata), edata, GL_STATIC_DRAW);

	//setup framebuffer for deferred rendering
	auto& albedo = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_albedo", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::BYTE_RGBA);
	auto& normal = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_normal", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::FLOAT_RGB);
	auto& depth = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_depth", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::DEPTH_24);
	albedo.create(1280, 720, nullptr);
	normal.create(1280, 720, nullptr);
	depth.create(1280, 720, nullptr);


	//gbuffer
	gbuffer_.addTexture(albedo, oak::graphics::FramebufferAttachment::COLOR0, 0);
	gbuffer_.addTexture(normal, oak::graphics::FramebufferAttachment::COLOR1, 0);
	gbuffer_.addTexture(depth, oak::graphics::FramebufferAttachment::DEPTH, 0);

	gbuffer_.create(0, 0);
}

void DeferredRenderer::terminate() {
	light_.destroy();
	buffer_.destroy();
	gbuffer_.destroy();
}

void DeferredRenderer::render(oak::graphics::Api *api) {

	glEnable(GL_DEPTH_TEST);

	//do rendering stuff
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.getId());

	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	//render fullscreen quad
	glDisable(GL_DEPTH_TEST);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light_.getId());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_albedo").getId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_normal").getId());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_depth").getId());

	buffer_.bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}