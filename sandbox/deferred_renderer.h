#pragma once

#include <glad/glad.h>

#include <graphics/renderer.h>
#include <graphics/framebuffer.h>
#include <graphics/shader.h>
#include <graphics/buffer.h>
#include <graphics/opengl/gl_buffer_storage.h>

class DeferredRenderer : public oak::graphics::Renderer {
public:

	void init();
	void terminate();

	void render(oak::graphics::Api *api) override;
	inline void setPipeline(const oak::graphics::Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	const oak::graphics::Pipeline *pipeline_;
	oak::graphics::Framebuffer gbuffer_;
	oak::graphics::Framebuffer ssaobuffer_;
	oak::graphics::Framebuffer aabuffer_;
	oak::graphics::Shader light_;
	oak::graphics::Shader ssao_;
	oak::graphics::Shader fxaa_;
	oak::graphics::GLBufferStorage buffer_;
	oak::graphics::Buffer kernelBuffer_;
	oak::graphics::Texture albedo_;
	oak::graphics::Texture normal_;
	oak::graphics::Texture depth_;
	oak::graphics::Texture noise_;
	oak::graphics::Texture aa_;
	oak::graphics::Texture ao_;
};