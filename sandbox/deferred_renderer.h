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
	oak::graphics::Framebuffer saobuffer_;
	oak::graphics::Framebuffer post_;
	oak::graphics::Framebuffer cszBuffer_[5];
	oak::graphics::Framebuffer blurBuffer_[2];
	oak::graphics::Shader light_;
	oak::graphics::Shader sao_;
	oak::graphics::Shader reconstruct_;
	oak::graphics::Shader minify_;
	oak::graphics::Shader aaBlur_;
	oak::graphics::Shader fxaa_;
	oak::graphics::Shader box_;
	oak::graphics::GLBufferStorage buffer_;
	oak::graphics::GLBufferStorage skyBuffer_;
	oak::graphics::Texture albedo_;
	oak::graphics::Texture normal_;
	oak::graphics::Texture depth_;
	oak::graphics::Texture aa_;
	oak::graphics::Texture ao_;
	oak::graphics::Texture sky_;
	oak::graphics::Texture csz_;
	oak::graphics::Texture blur_[2];
};