#include "gl_framebuffer.h"

#include <glad/glad.h>

#include "graphics/framebuffer.h"
#include "log.h"

namespace oak::graphics::GLFramebuffer {

	Framebuffer create(const FramebufferInfo& info) {

		GLuint fbo = 0, rbo = 0;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//make sure that the width and hight are not zero
		if (info.useRenderbuffer) {
			GLenum format = 0, attachment = 0;
			if (info.renderBuffer == FramebufferAttachment::DEPTH) {
				format = GL_DEPTH_COMPONENT24;
				attachment = GL_DEPTH_ATTACHMENT;
			}
			if (info.renderBuffer == FramebufferAttachment::DEPTH_STENCIL) {
				format = GL_DEPTH24_STENCIL8;
				attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			}
			//create the render buffer
			if (format) {
				glGenRenderbuffers(1, &rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, format, info.width, info.height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
			}
		}

		for (const auto& info : info.attachments) {
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<int>(info.attachment), info.tex->id, info.mipmap);
		}

		//fill draw buffers array
		oak::vector<GLenum> drawBuffers;

		for (size_t i = 0; i < info.attachments.size(); i++) {
			if (info.attachments[i].attachment == FramebufferAttachment::DEPTH || info.attachments[i].attachment == FramebufferAttachment::DEPTH_STENCIL) { continue; }
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + static_cast<int>(info.attachments[i].attachment));
		}
		//set the draw buffers
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		Framebuffer fb{ fbo, rbo, info };

		GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result != GL_FRAMEBUFFER_COMPLETE) {
			log_print_warn("ruuun!!! the world is ending.... FRAMEBUFFER EXPLODING!!.!.!..!");
			destroy(fb);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return fb;
	}

	void destroy(Framebuffer& framebuffer) {
		if (framebuffer.id) {
			glDeleteFramebuffers(1, &framebuffer.id);
			framebuffer.id = 0;
		}
		if (framebuffer.rid) {
			glDeleteRenderbuffers(1, &framebuffer.rid);
			framebuffer.rid = 0;
		}
	}

	void bind(const Framebuffer& framebuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}