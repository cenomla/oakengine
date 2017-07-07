#include <graphics/renderer.h>
#include <graphics/opengl/gl_framebuffer.h>
#include <graphics/opengl/gl_shader.h>
#include <graphics/opengl/gl_buffer_storage.h>

class TestRenderer : public oak::graphics::Renderer {
public:

	void init();
	void terminate();

	void render(oak::graphics::Api *api) override;
	inline void setPipeline(oak::graphics::Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	oak::graphics::Pipeline *pipeline_;
	oak::graphics::GLFramebuffer gbuffer_;
	oak::graphics::GLShader light_;
	oak::graphics::GLBufferStorage buffer_;
};