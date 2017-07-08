#include <graphics/renderer.h>
#include <graphics/opengl/gl_framebuffer.h>
#include <graphics/opengl/gl_shader.h>
#include <graphics/opengl/gl_buffer_storage.h>

class DeferredRenderer : public oak::graphics::Renderer {
public:

	void init();
	void terminate();

	void render(oak::graphics::Api *api) override;
	inline void setPipeline(const oak::graphics::Pipeline *pipeline) override { pipeline_ = pipeline; }

private:
	const oak::graphics::Pipeline *pipeline_;
	oak::graphics::GLFramebuffer gbuffer_;
	oak::graphics::GLFramebuffer ssaobuffer_;
	oak::graphics::GLShader light_;
	oak::graphics::GLShader ssao_;
	oak::graphics::GLBufferStorage buffer_;
	oak::graphics::GLBuffer kernelBuffer_{ GL_UNIFORM_BUFFER };
};