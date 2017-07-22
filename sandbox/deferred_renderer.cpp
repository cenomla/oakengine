#include "deferred_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <graphics/material.h>
#include <graphics/opengl/gl_api.h>
#include <graphics/pipeline.h>
#include <input_manager.h>

#include <random>

void DeferredRenderer::init() {

	//create shaders
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "core/graphics/shaders/deferred/sao/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/sao/ao.glsl";
	sao_ = oak::graphics::shader::create(shaderInfo);
	//set sao uniforms
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), (1920.0f)/(1080.0f), 0.5f, 500.0f);
	oak::graphics::shader::bind(sao_); 
	oak::graphics::shader::setUniform(sao_, "projInfo", 
	glm::vec4{ 
		-2.0f / ((1920.0f) * proj[0][0]), 
		-2.0f / ((1080.0f) * proj[1][1]),
		(1.0f - proj[0][2]) / proj[0][0],
		(1.0f + proj[1][2]) / proj[1][1]
	});
	oak::graphics::shader::setUniform(sao_, "projScale", 256.0f);
	oak::graphics::shader::setUniform(sao_, "CS_Z_buffer", 2); //depth texture location
	oak::graphics::shader::setUniform(sao_, "radius", 10.0f); //radius in meters
	oak::graphics::shader::setUniform(sao_, "bias", 0.012f);
	oak::graphics::shader::setUniform(sao_, "intensityDivR6", 4.0f / powf(10.0f, 6.0f));
	oak::graphics::shader::unbind();
	shaderInfo.fragment = "core/graphics/shaders/deferred/sao/reconstruct.glsl";
	reconstruct_ = oak::graphics::shader::create(shaderInfo);
	oak::graphics::shader::bind(reconstruct_);
	oak::graphics::shader::setUniform(reconstruct_, "clipInfo", glm::vec3{ 0.5f * 500.0f, 0.5f - 500.0f, 500.0f });
	oak::graphics::shader::unbind();
	shaderInfo.fragment = "core/graphics/shaders/deferred/sao/minify.glsl";
	minify_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/light/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/light/frag.glsl";
	light_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/fxaa/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/fxaa/frag.glsl";
	fxaa_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/box/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/box/frag.glsl";
	box_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/sao/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/sao/blur.glsl";
	aaBlur_ = oak::graphics::shader::create(shaderInfo);
	
	oak::graphics::AttributeLayout layoutScreen{ oak::vector<oak::graphics::AttributeType>{ 
		oak::graphics::AttributeType::POSITION2D
	} };
	buffer_.create(&layoutScreen);

	const float vdata[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f, 
		1.0f, 1.0f,
		-1.0f, 1.0f
	};

	const unsigned int edata[] = {
		0, 1, 2, 2, 3, 0
	};

	buffer_.bind();
	buffer_.data(0, sizeof(vdata), vdata);
	buffer_.data(1, sizeof(edata), edata);
	buffer_.unbind();

	oak::graphics::AttributeLayout layoutSky{ oak::vector<oak::graphics::AttributeType>{
		oak::graphics::AttributeType::POSITION
	} };
	skyBuffer_.create(&layoutSky);

	const float sdata[] = {
		 // positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	skyBuffer_.bind();
	skyBuffer_.data(0, sizeof(sdata), sdata);
	skyBuffer_.unbind();


	oak::graphics::TextureInfo texInfo;
	texInfo.minFilter = oak::graphics::TextureFilter::NEAREST;
	texInfo.magFilter = oak::graphics::TextureFilter::NEAREST;
	texInfo.xWrap = oak::graphics::TextureWrap::CLAMP_EDGE;
	texInfo.yWrap = oak::graphics::TextureWrap::CLAMP_EDGE;
	//setup framebuffer for deferred rendering
	texInfo.format = oak::graphics::TextureFormat::BYTE_RGBA;
	texInfo.width = pipeline_->width + 512;
	texInfo.height = pipeline_->height + 512;
	albedo_ = oak::graphics::texture::create(texInfo, nullptr);
	normal_ = oak::graphics::texture::create(texInfo, nullptr);
	//create depth textures
	texInfo.format = oak::graphics::TextureFormat::DEPTH_24;
	depth_ = oak::graphics::texture::create(texInfo, nullptr);
	texInfo.mipLevels = 5;
	texInfo.format = oak::graphics::TextureFormat::FLOAT_R;
	csz_ = oak::graphics::texture::create(texInfo, nullptr);
	//post process textures
	texInfo.mipLevels = 1;
	texInfo.format = oak::graphics::TextureFormat::BYTE_RGB;
	ao_ = oak::graphics::texture::create(texInfo, nullptr);
	blur_[0] = oak::graphics::texture::create(texInfo, nullptr);
	blur_[1] = oak::graphics::texture::create(texInfo, nullptr);
	texInfo.format = oak::graphics::TextureFormat::BYTE_RGBA;
	aa_ = oak::graphics::texture::create(texInfo, nullptr);

	texInfo.type = oak::graphics::TextureType::CUBEMAP;
	texInfo.format = oak::graphics::TextureFormat::BYTE_RGBA;
	texInfo.magFilter = oak::graphics::TextureFilter::LINEAR;
	texInfo.minFilter = oak::graphics::TextureFilter::LINEAR;
	sky_ = oak::graphics::texture::createCubemap({
		"sandbox/res/textures/envmap_ocean/right.jpg",
		"sandbox/res/textures/envmap_ocean/left.jpg",
		"sandbox/res/textures/envmap_ocean/top.jpg",
		"sandbox/res/textures/envmap_ocean/bottom.jpg",
		"sandbox/res/textures/envmap_ocean/back.jpg",
		"sandbox/res/textures/envmap_ocean/front.jpg"
	}, texInfo);

	//ssao buffer
	oak::graphics::FramebufferInfo framebufferInfo;
	framebufferInfo.useRenderbuffer = false;
	framebufferInfo.width = pipeline_->width;
	framebufferInfo.height = pipeline_->height;
	framebufferInfo.attachments.push_back({ &ao_, oak::graphics::FramebufferAttachment::COLOR0 });

	saobuffer_ = oak::graphics::framebuffer::create(framebufferInfo);

	//gbuffer
	framebufferInfo.attachments.clear();
	framebufferInfo.attachments.push_back({ &albedo_, oak::graphics::FramebufferAttachment::COLOR0 });
	framebufferInfo.attachments.push_back({ &normal_, oak::graphics::FramebufferAttachment::COLOR1 });
	framebufferInfo.attachments.push_back({ &depth_, oak::graphics::FramebufferAttachment::DEPTH });

	gbuffer_ = oak::graphics::framebuffer::create(framebufferInfo);

	//post buffer
	framebufferInfo.attachments.clear();
	framebufferInfo.attachments.push_back({ &aa_, oak::graphics::FramebufferAttachment::COLOR0 });
	framebufferInfo.attachments.push_back({ &depth_, oak::graphics::FramebufferAttachment::DEPTH });

	post_ = oak::graphics::framebuffer::create(framebufferInfo);

	//csz

	for (int i = 0; i < 5; i++) {
		framebufferInfo.attachments.clear();
		framebufferInfo.attachments.push_back({ &csz_, oak::graphics::FramebufferAttachment::COLOR0, i });
		cszBuffer_[i] = oak::graphics::framebuffer::create(framebufferInfo);
	}

	//blur
	framebufferInfo.attachments.clear();
	framebufferInfo.attachments.push_back({ &blur_[0], oak::graphics::FramebufferAttachment::COLOR0 });

	blurBuffer_[0] = oak::graphics::framebuffer::create(framebufferInfo);

	framebufferInfo.attachments.clear();
	framebufferInfo.attachments.push_back({ &blur_[1], oak::graphics::FramebufferAttachment::COLOR0 });

	blurBuffer_[1] = oak::graphics::framebuffer::create(framebufferInfo);

}

void DeferredRenderer::terminate() {
	buffer_.destroy();
	skyBuffer_.destroy();

	gbuffer_.destroy();
	saobuffer_.destroy();
	post_.destroy();
	for (int i = 0; i < 5; i++) {
		cszBuffer_[i].destroy();
	}
	for (int i = 0; i < 2; i++) {
		blurBuffer_[i].destroy();
	}
	light_.destroy();
	sao_.destroy();
	reconstruct_.destroy();
	minify_.destroy();
	aaBlur_.destroy();
	fxaa_.destroy();
	box_.destroy();

	albedo_.destroy();
	normal_.destroy();
	depth_.destroy();
	aa_.destroy();
	ao_.destroy();
	sky_.destroy();
	csz_.destroy();
	for (int i = 0; i < 2; i++) {
		blur_[i].destroy();
	}
}

void DeferredRenderer::render(oak::graphics::Api *api) {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width + 512, pipeline_->height + 512);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//do rendering stuff
	oak::graphics::framebuffer::bind(gbuffer_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& batch : *pipeline_->batches) {
		if (batch.layer != 0) { continue; }
		//bind material 
		oak::graphics::shader::bind(*batch.material->shader);
		for (int i = 0; i < 16; i++) {
			if (batch.material->textures[i] != nullptr) {
				oak::graphics::texture::bind(*batch.material->textures[i], i);
			}
		}
		batch.storage->bind();
		//render stuff

		if (batch.instances > 0) {
			glDrawElementsInstanced(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4), batch.instances);
		} else {
			glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));
		}
	}

	//set opengl state
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	//bind textures
	oak::graphics::texture::bind(albedo_, 0);
	oak::graphics::texture::bind(normal_, 1);
	oak::graphics::texture::bind(depth_, 2);

	//bind the fullscreen quad buffer
	buffer_.bind();

	//prerender sao
	oak::graphics::framebuffer::bind(cszBuffer_[0]);
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::shader::bind(reconstruct_);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	oak::graphics::texture::bind(csz_, 2);
	oak::graphics::shader::bind(minify_);

	for (int i = 1; i < 5 ; i++) {
		oak::graphics::framebuffer::bind(cszBuffer_[i]);
		glClear(GL_COLOR_BUFFER_BIT);
		
		oak::graphics::shader::setUniform(minify_, "previousMIPNumber", i - 1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//render ssao
	//now only rendering to screen
	glEnable(GL_SCISSOR_TEST);
	glScissor(256, 256, pipeline_->width, pipeline_->height);
	oak::graphics::framebuffer::bind(blurBuffer_[0]);
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::shader::bind(sao_);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//blur pass
	oak::graphics::framebuffer::bind(blurBuffer_[1]);
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::shader::bind(aaBlur_);
	oak::graphics::shader::setUniform(aaBlur_, "axis", glm::ivec2{ 0, 1 });
	oak::graphics::texture::bind(blur_[0], 3);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	oak::graphics::framebuffer::bind(saobuffer_);
	glClear(GL_COLOR_BUFFER_BIT);
	
	oak::graphics::shader::setUniform(aaBlur_, "axis", glm::ivec2{ 1, 0 });
	oak::graphics::texture::bind(blur_[1], 3);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//render lighting
	oak::graphics::framebuffer::bind(post_);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind ao texture to slot 3
	oak::graphics::texture::bind(ao_, 3);
	oak::graphics::texture::bind(depth_, 2);

	oak::graphics::shader::bind(light_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//render skybox
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	oak::graphics::shader::bind(box_);
	oak::graphics::texture::bind(sky_, 0);

	skyBuffer_.bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	
	
	//aa
	oak::graphics::framebuffer::unbind();
	glViewport(pipeline_->x - 256, pipeline_->y - 256, pipeline_->width + 512, pipeline_->height + 512);
	glScissor(0, 0, pipeline_->width, pipeline_->height);
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::texture::bind(aa_, 0);
	oak::graphics::shader::bind(fxaa_);

	buffer_.bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_SCISSOR_TEST);
	oak::graphics::vertex_array::unbind();
}