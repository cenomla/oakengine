#pragma once

#include <graphics/renderer.h>
#include <graphics/opengl/gl_framebuffer.h>
#include <graphics/opengl/gl_shader.h>
#include <graphics/opengl/gl_buffer_storage.h>


class GuiRenderer : public oak::graphics::Renderer {
public:
	void init();
	void terminate();

	void render(oak::graphics::Api *api) override;
	inline void setPipeline(const oak::graphics::Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	const oak::graphics::Pipeline *pipeline_;
};