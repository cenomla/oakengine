#include "gl_texture.h"

#include <glad/glad.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "graphics/texture.h"
#include "log.h"

namespace oak::graphics {

	static GLenum formats[][3] = {
		{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
		{ GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE },
		{ GL_R8, GL_RED, GL_UNSIGNED_BYTE },
		{ GL_RGBA32F, GL_RGBA, GL_FLOAT }, 
		{ GL_RGB32F, GL_RGB, GL_FLOAT },
		{ GL_R32F, GL_RED, GL_FLOAT },
		{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT },
		{ GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
		{ GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
		{ GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT }
	};

	static GLenum types[] = {
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_2D_ARRAY
	};

	static GLenum filter[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR
	};

	static GLenum wrap[] = {
		GL_REPEAT,
		GL_CLAMP_TO_EDGE
	};

	void GLTexture::bind(const Texture& texture, int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(types[static_cast<int>(texture.info.type)], texture.id);
	}

	Texture GLTexture::create(const char *path, int rcomp) {
		int w, h, comp;
		stbi_uc *data = stbi_load(path, &w, &h, &comp, rcomp);

		if (data == nullptr) {
			log_print_warn("failed to load texture: %s", path);
		}

		TextureInfo info;
		if (rcomp == 1) {
			info.format = TextureFormat::BYTE_R;
		}
		info.minFilter = TextureFilter::LINEAR_MIP_LINEAR;
		info.xWrap = TextureWrap::REPEAT;
		info.yWrap = TextureWrap::REPEAT;
		info.width = w;
		info.height = h;

		const Texture& tex = create(info, data);
		
		GLfloat fl;

		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fl);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fl);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		return tex;
	}

	Texture GLTexture::create(const TextureInfo& info, void *data) {

		const auto& format = formats[static_cast<int>(info.format)];

		GLenum type = types[static_cast<int>(info.type)];
		GLenum mag = filter[static_cast<int>(info.magFilter)];
		GLenum min = filter[static_cast<int>(info.minFilter)];

		GLenum xw = wrap[static_cast<int>(info.xWrap)];
		GLenum yw = wrap[static_cast<int>(info.yWrap)];

		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(type, tex);
		glTexImage2D(type, 0, format[0], info.width, info.height, 0, format[1], format[2], data);
		
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, mag);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, xw);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, yw);

		return { tex, info }; 
	}

	void GLTexture::destroy(Texture& texture) {
		if (texture.id) {
			glDeleteTextures(1, &texture.id);
			texture.id = 0;
		}
	}


}