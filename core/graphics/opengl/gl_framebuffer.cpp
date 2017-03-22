#include "gl_framebuffer.h"

#include "log.h"

namespace oak::graphics {

	GLFramebuffer::GLFramebuffer() : fbo_{ 0 }, rbo_{ 0 } {

	}

	GLFramebuffer::~GLFramebuffer() {
		destroy();
	}

	void GLFramebuffer::create(const GLTexture &texture, int width, int height) {
		if (fbo_ != 0) { return; }

		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

		glGenRenderbuffers(1, &rbo_);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.getId(), 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

		GLenum drawBuffers[] = {
			GL_COLOR_ATTACHMENT0
		};
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			log_print_warn("ruuun!!! the world is ending.... FRAMEBUFFER EXPLODING!!.!.!..!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::destroy() {
		if (fbo_ != 0) {
			glDeleteFramebuffers(1, &fbo_);
			glDeleteRenderbuffers(1, &rbo_);
			fbo_ = 0;
			rbo_ = 0;
		}
	}

	void GLFramebuffer::bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	}

	void GLFramebuffer::unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}