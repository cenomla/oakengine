#include "deferred_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <graphics/material.h>
#include <graphics/opengl/gl_api.h>
#include <graphics/pipeline.h>
#include <input_manager.h>

#include <random>

void DeferredRenderer::init() {
	std::random_device rd;
 
	std::mt19937 re{ rd() };

	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	struct {
		glm::vec4 samples[128];
		float radius = 1.27;
		float power = 4.0;
		int count = 16;
	} kernel;
	for (size_t i = 0; i < 128; i ++) {
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

	oak::graphics::TextureInfo texInfo;
	texInfo.format = oak::graphics::TextureFormat::FLOAT_RGB;
	texInfo.width = 4;
	texInfo.height = 4;
	texInfo.minFilter = oak::graphics::TextureFilter::NEAREST;
	texInfo.magFilter = oak::graphics::TextureFilter::NEAREST;
	texInfo.xWrap = oak::graphics::TextureWrap::CLAMP_EDGE;
	texInfo.yWrap = oak::graphics::TextureWrap::CLAMP_EDGE;
	noise_ = oak::graphics::texture::create(texInfo, noise.data());


	//create kernel buffer
	oak::graphics::BufferInfo bufferInfo;
	bufferInfo.base = 3;
	bufferInfo.type = oak::graphics::BufferType::UNIFORM;
	bufferInfo.hint = oak::graphics::BufferHint::STATIC;
	kernelBuffer_ = oak::graphics::buffer::create(bufferInfo);
	oak::graphics::buffer::bind(kernelBuffer_);
	oak::graphics::buffer::data(kernelBuffer_, sizeof(kernel), &kernel);

	//create shaders
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "core/graphics/shaders/deferred/ssao/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/ssao/frag.glsl";
	ssao_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/light/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/light/frag.glsl";
	light_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/fxaa/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/fxaa/frag.glsl";
	fxaa_ = oak::graphics::shader::create(shaderInfo);
	shaderInfo.vertex = "core/graphics/shaders/deferred/box/vert.glsl";
	shaderInfo.fragment = "core/graphics/shaders/deferred/box/frag.glsl";
	box_ = oak::graphics::shader::create(shaderInfo);
	
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

	//setup framebuffer for deferred rendering
	texInfo.format = oak::graphics::TextureFormat::BYTE_RGBA;
	texInfo.width = pipeline_->width;
	texInfo.height = pipeline_->height;
	albedo_ = oak::graphics::texture::create(texInfo, nullptr);
	normal_ = oak::graphics::texture::create(texInfo, nullptr);
	aa_ = oak::graphics::texture::create(texInfo, nullptr);
	texInfo.format = oak::graphics::TextureFormat::DEPTH_24;
	depth_ = oak::graphics::texture::create(texInfo, nullptr);
	texInfo.format = oak::graphics::TextureFormat::BYTE_R;
	ao_ = oak::graphics::texture::create(texInfo, nullptr);

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

	ssaobuffer_ = oak::graphics::framebuffer::create(framebufferInfo);

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
}

void DeferredRenderer::terminate() {
	oak::graphics::shader::destroy(light_);
	oak::graphics::shader::destroy(ssao_);
	oak::graphics::shader::destroy(fxaa_);
	oak::graphics::shader::destroy(box_);
	buffer_.destroy();
	skyBuffer_.destroy();
	oak::graphics::buffer::destroy(kernelBuffer_);
	oak::graphics::framebuffer::destroy(gbuffer_);
	oak::graphics::framebuffer::destroy(ssaobuffer_);
	oak::graphics::framebuffer::destroy(post_);

	oak::graphics::texture::destroy(albedo_);
	oak::graphics::texture::destroy(normal_);
	oak::graphics::texture::destroy(depth_);
	oak::graphics::texture::destroy(noise_);
	oak::graphics::texture::destroy(aa_);
	oak::graphics::texture::destroy(ao_);
	oak::graphics::texture::destroy(sky_);
}

void DeferredRenderer::render(oak::graphics::Api *api) {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(pipeline_->x, pipeline_->y, pipeline_->width, pipeline_->height);
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
	oak::graphics::texture::bind(noise_, 3);

	//bind the fullscreen quad buffer
	buffer_.bind();

	//render ssao
	oak::graphics::framebuffer::bind(ssaobuffer_);
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::shader::bind(ssao_);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//render lighting
	oak::graphics::framebuffer::bind(post_);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind ao texture to slot 3
	oak::graphics::texture::bind(ao_, 3);

	oak::graphics::shader::bind(light_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//render skybox
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	oak::graphics::shader::bind(box_);
	oak::graphics::texture::bind(sky_, 0);

	skyBuffer_.bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisable(GL_DEPTH_TEST);
	
	
	//aa
	oak::graphics::framebuffer::unbind();
	glClear(GL_COLOR_BUFFER_BIT);

	oak::graphics::texture::bind(aa_, 0);
	oak::graphics::shader::bind(fxaa_);

	buffer_.bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	oak::graphics::vertex_array::unbind();
	
}