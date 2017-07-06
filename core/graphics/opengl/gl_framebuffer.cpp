#include "gl_framebuffer.h"

#include "log.h"

namespace oak::graphics {

	GLFramebuffer::GLFramebuffer() : fbo_{ 0 }, rbo_{ 0 } {

	}

	GLFramebuffer::~GLFramebuffer() {
		destroy();
	}

	void GLFramebuffer::addTexture(const GLTexture &texture, int mipmap) {
		if (fbo_ != 0) { return; } //make sure the frame buffer has not been created

		textures_.push_back({ &texture, mipmap });
	}

	void GLFramebuffer::create(int width, int height) {
		if (fbo_ != 0) { return; }

		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

		//create the render buffer
		glGenRenderbuffers(1, &rbo_);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

		int i = 0;
		for (const auto& info : textures_) {
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, info.tex->getId(), info.mipmap);
			i++;
		}

		//fill draw buffers array
		oak::vector<GLenum> drawBuffers;
		drawBuffers.resize(textures_.size());

		for (i = 0; i < drawBuffers.size(); i++) {
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		//set the draw buffers
		glDrawBuffers(1, drawBuffers.data());

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