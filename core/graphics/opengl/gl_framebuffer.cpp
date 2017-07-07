#include "gl_framebuffer.h"

#include "log.h"

namespace oak::graphics {

	GLFramebuffer::GLFramebuffer() : fbo_{ 0 }, rbo_{ 0 } {

	}

	GLFramebuffer::~GLFramebuffer() {
		destroy();
	}

	void GLFramebuffer::addTexture(const GLTexture &texture, FramebufferAttachment attachment, int mipmap) {
		if (fbo_ != 0) { return; } //make sure the frame buffer has not been created

		textures_.push_back({ &texture, attachment, mipmap });
	}

	void GLFramebuffer::create(int width, int height) {
		if (fbo_ != 0) { return; }

		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

		//make sure that the width and hight are not zero
		if (width != 0 && height != 0) {
			//create the render buffer
			glGenRenderbuffers(1, &rbo_);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);
		}

		for (const auto& info : textures_) {
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(info.attachment), info.tex->getId(), info.mipmap);
		}

		//fill draw buffers array
		oak::vector<GLenum> drawBuffers;

		for (size_t i = 0; i < textures_.size(); i++) {
			if (textures_[i].attachment == FramebufferAttachment::DEPTH || textures_[i].attachment == FramebufferAttachment::DEPTH_STENCIL) { continue; }
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + static_cast<int>(textures_[i].attachment));
		}
		//set the draw buffers
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (result != GL_FRAMEBUFFER_COMPLETE) {
			log_print_warn("ruuun!!! the world is ending.... FRAMEBUFFER EXPLODING!!.!.!..!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::destroy() {
		if (fbo_ != 0) {
			glDeleteFramebuffers(1, &fbo_);
			fbo_ = 0;
		}
		if (rbo_ != 0) {
			glDeleteRenderbuffers(1, &rbo_);
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