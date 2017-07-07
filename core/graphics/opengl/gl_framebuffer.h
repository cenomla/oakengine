#pragma once

#include <glad/glad.h>

#include "container.h"
#include "gl_texture.h"

#include "graphics/api.h"

namespace oak::graphics {

	class GLFramebuffer {
	public:
		GLFramebuffer();
		~GLFramebuffer();

		void addTexture(const GLTexture &texture, FramebufferAttachment attachment, int mipmap);

		void create(int width, int height);
		void destroy();

		void bind() const;
		void unbind() const;

		inline uint32_t getId() const { return fbo_; }

	private:
		struct TexTargetInfo {
			const GLTexture *tex;
			FramebufferAttachment attachment;
			int mipmap;	
		};

		GLuint fbo_;
		GLuint rbo_;
		oak::vector<TexTargetInfo> textures_;
	};

}