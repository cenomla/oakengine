#include "gl_texture.h"

#include <cmath>

#include <glad/glad.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>


#include "graphics/texture.h"
#include "log.h"

namespace oak::graphics::GLTexture {

	static const GLenum formats[][3] = {
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

	static const int components[] = {
		4,
		3,
		1,
		4,
		3,
		1,
		1,
		1,
		1,
		1
	};

	static const GLenum types[] = {
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_2D_ARRAY
	};

	static const GLenum filter[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR
	};

	static const GLenum wrap[] = {
		GL_REPEAT,
		GL_CLAMP_TO_EDGE
	};

	void bind(const Texture& texture, int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(types[static_cast<int>(texture.info.type)], texture.id);
	}

	Texture create(const char *path, TextureInfo& info) {
		int w, h, comp;
		stbi_uc *data = stbi_load(path, &w, &h, &comp, components[static_cast<int>(info.format)]);

		if (data == nullptr) {
			log_print_warn("failed to load texture: %s", path);
		}

		info.width = w;
		info.height = h;

		const Texture& tex = create(info, data);

		if (static_cast<int>(info.minFilter) >= static_cast<int>(TextureFilter::LINEAR_MIP_LINEAR)) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);

		return tex;
	}

	Texture create(const TextureInfo& info, void *data) {

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

	TextureAtlas createAtlas(const oak::vector<const char*>& paths, const TextureInfo& info) {
		const auto& format = formats[static_cast<int>(info.format)];

		GLenum type = types[static_cast<int>(info.type)];
		GLenum mag = filter[static_cast<int>(info.magFilter)];
		GLenum min = filter[static_cast<int>(info.minFilter)];

		GLenum xw = wrap[static_cast<int>(info.xWrap)];
		GLenum yw = wrap[static_cast<int>(info.yWrap)];

		int rcomp = components[static_cast<int>(info.format)];

		struct ImageInfo {
			const char *name;
			stbi_uc *data;
			int width, height, comp;
		};

		oak::vector<ImageInfo> images;

		//load textures
		for (auto path : paths) {
			int w, h, c;
			stbi_uc *data = stbi_load(path, &w, &h, &c, rcomp);
			images.push_back({ path, data, w, h, c});
		}

		TextureAtlas atlas;
		atlas.regions.resize(images.size());

		//pack rects
		oak::vector<stbrp_node> nodes;
		nodes.resize(info.width+1);

		stbrp_context c;
		stbrp_init_target(&c, info.width, info.height, nodes.data(), nodes.size());

		oak::vector<stbrp_rect> rects;
		rects.resize(images.size());

		for (size_t i = 0; i < rects.size(); i++) {
			auto& rect = rects[i];
			const auto& it = images[i];
			rect.id = i;
			rect.w = it.width;
			rect.h = it.height;
		}

		stbrp_pack_rects(&c, rects.data(), rects.size());

		//create texture and upload it to opengl
		Texture texture;
		texture.info = info;
		glGenTextures(1, &texture.id);
		glBindTexture(type, texture.id);
		glTexStorage2D(type, 8, format[0], info.width, info.height);

		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, mag);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, xw);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, yw);

		float tx = 1.0f / info.width;
		float ty = 1.0f / info.height;

		if (static_cast<int>(info.minFilter) >= static_cast<int>(TextureFilter::LINEAR_MIP_LINEAR)) {
			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(type, tex);
			for (const auto& rect : rects) {
				const auto& image = images[rect.id];
				auto& region = atlas.regions[rect.id];
				region.first = oak::string{ image.name };

				region.second.pos = { rect.x * tx, rect.y * ty };
				region.second.extent = { rect.w * ty, rect.h * ty };

				glTexImage2D(type, 0, format[0], image.width, image.height, 0, format[1], format[2], image.data);
				glGenerateMipmap(type);

				int mipLevels = 1 + floor(log2(std::max(image.width, image.height)));

				for (int i = 0; i < mipLevels && i < 8; i++) {
					glCopyImageSubData(tex, type, i, 0, 0, 0, texture.id, type, i, rect.x >> i, rect.y >> i, 0, image.width >> i, image.height >> i, 1);
				}
			}
			glDeleteTextures(1, &tex);
		} else {
			for (const auto& rect : rects) {
				const auto& image = images[rect.id];
				auto& region = atlas.regions[rect.id];
				region.first = oak::string{ image.name };

				region.second.pos = { rect.x * tx, rect.y * ty };
				region.second.extent = { rect.w * ty, rect.h * ty };

				glTexSubImage2D(type, 0, rect.x, rect.y, rect.w, rect.h, format[1], format[2], image.data);
			}
		}
		
		//free image data
		for (auto& image : images) {
			stbi_image_free(image.data);
		}

		atlas.texture = texture;

		return atlas;
	}

	void destroy(Texture& texture) {
		if (texture.id) {
			glDeleteTextures(1, &texture.id);
			texture.id = 0;
		}
	}


}