#include "deferred_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include <glad/glad.h>
#include <graphics/material.h>
#include <graphics/api.h>
#include <graphics/pipeline.h>
#include <graphics/opengl/gl_texture.h>

#include <resource_manager.h>

#include <random>

void DeferredRenderer::init() {
	std::random_device rd;

	std::mt19937 re{ rd() };

	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	struct {
		glm::vec4 samples[64];
		float radius = 1.67;
	} kernel;
	for (size_t i = 0; i < 64; i ++) {
		kernel.samples[i] = {
			dist(re) * 2.0f - 1.0f,
			dist(re) * 2.0f - 1.0f,
			dist(re), 1.0f
		};
		kernel.samples[i] *= dist(re);
		float scale = static_cast<float>(i) / 64.0f;
		scale = glm::lerp(0.1f, 1.0f, scale * scale);
		kernel.samples[i] *= scale;
		kernel.samples[i] = glm::normalize(kernel.samples[i]);
	}

	oak::vector<glm::vec3> noise;
	noise.resize(16);

	for (size_t i = 0; i < noise.size(); i++) {
		noise[i] = {
			dist(re) * 2.0f - 1.0f,
			dist(re) * 2.0f - 1.0f,
			0.0f
		};
		noise[i] = glm::normalize(noise[i]);
	}

	auto& gltex_noise = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_noise", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::FLOAT_RGB);
	gltex_noise.create(4, 4, noise.data());


	//create kernel buffer
	kernelBuffer_.create();
	kernelBuffer_.bindBufferBase(3);
	kernelBuffer_.data(sizeof(kernel), &kernel, GL_STATIC_DRAW);

	ssao_.create("core/graphics/shaders/deferred/ssao/vert.glsl", "core/graphics/shaders/deferred/ssao/frag.glsl");

	ssao_.bind();
	ssao_.setUniform("texNormal", 1);
	ssao_.setUniform("texDepth", 2);
	ssao_.setUniform("texNoise", 3);

	ssao_.bindBlockIndex("MatrixBlock", 2);
	ssao_.bindBlockIndex("KernelBlock", 3);

	light_.create("core/graphics/shaders/deferred/light/vert.glsl", "core/graphics/shaders/deferred/light/frag.glsl");

	light_.bind();
	light_.setUniform("texAlbedo", 0);
	light_.setUniform("texNormal", 1);
	light_.setUniform("texDepth", 2);
	light_.setUniform("texAo", 3);

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
	auto& albedo = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_albedo", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::BYTE_RGBA, GLenum{ GL_NEAREST }, GLenum{ GL_CLAMP_TO_EDGE });
	auto& normal = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_normal", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::FLOAT_RGBA, GLenum{ GL_NEAREST }, GLenum{ GL_CLAMP_TO_EDGE });
	auto& depth = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_depth", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::DEPTH_24, GLenum{ GL_NEAREST }, GLenum{ GL_CLAMP_TO_EDGE });
	auto& ao = oak::ResourceManager::inst().add<oak::graphics::GLTexture>("gltex_ao", GLuint{ GL_TEXTURE_2D }, oak::graphics::TextureFormat::FLOAT_R, GLenum{ GL_NEAREST }, GLenum{ GL_CLAMP_TO_EDGE });
	albedo.create(1280, 720, nullptr);
	normal.create(1280, 720, nullptr);
	depth.create(1280, 720, nullptr);
	ao.create(1280, 720, nullptr);

	//ssao buffer
	ssaobuffer_.addTexture(ao, oak::graphics::FramebufferAttachment::COLOR0, 0);
	ssaobuffer_.create(0, 0);

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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

	//set opengl state
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	//bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_albedo").getId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_normal").getId());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_depth").getId());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_noise").getId());

	//bind the fullscreen quad buffer
	buffer_.bind();

	//render ssao
	glBindFramebuffer(GL_FRAMEBUFFER, ssaobuffer_.getId());
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(ssao_.getId());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind ao texture to slot 3
	glBindTexture(GL_TEXTURE_2D, oak::ResourceManager::inst().require<oak::graphics::GLTexture>("gltex_ao").getId());

	glUseProgram(light_.getId());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}