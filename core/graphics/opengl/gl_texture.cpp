#include "gl_texture.h"


#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.h"

namespace oak::graphics {

	GLTexture::GLTexture(GLenum type) : tex_{ 0 }, type_{ type } {

	}

	GLTexture::~GLTexture() {
		destroy();
	}

	void GLTexture::bind(GLenum slot) const {
		glActiveTexture(slot);
		glBindTexture(type_, tex_);
	}

	void GLTexture::unbind() const {
		glBindTexture(type_, 0);
	}

	void GLTexture::create(const std::string &path) {
		if (tex_ != 0) { return; }

		int w, h, comp;
		stbi_uc *data = stbi_load(path.c_str(), &w, &h, &comp, 4);

		if (data == nullptr) {
			log::cout << "failed to load texture: " << path << std::endl;
		}

		glGenTextures(1, &tex_);
		glBindTexture(type_, tex_);
		glTexImage2D(type_, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		glTexParameteri(type_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(type_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(type_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(type_, 0);

		stbi_image_free(data);
	}

	void GLTexture::destroy() {
		if (tex_ != 0) {
			glDeleteTextures(1, &tex_);
			tex_ = 0;
		}
	}


}