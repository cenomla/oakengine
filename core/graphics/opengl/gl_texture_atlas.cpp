#include "gl_texture_atlas.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#include "log.h"

namespace oak::graphics {

	GLTextureAtlas::GLTextureAtlas(GLenum type, GLenum filter) : GLTexture{ type, filter } {
	}

	GLTextureAtlas::~GLTextureAtlas() {
		clear();
	}

	void GLTextureAtlas::addTexture(const std::string &name) {
		TextureImage image;
		image.name = name;
		image.data = stbi_load(name.c_str(), &image.width, &image.height, &image.comp, 4);

		if (image.data == nullptr) {
			log::cout << "failed to load texture: " << name << std::endl;
			abort();
		}

		regions_.push_back({ image, { 0.0f, 0.0f, 0.0f, 0.0f }});
	}

	void GLTextureAtlas::bake(int width, int height) {
		if (tex_ != 0) { return; }

		std::vector<stbrp_node> nodes;
		nodes.resize(width+1);

		stbrp_context c;
		stbrp_init_target(&c, width, height, nodes.data(), nodes.size());

		std::vector<stbrp_rect> rects;
		rects.resize(regions_.size());

		for (size_t i = 0; i < rects.size(); i++) {
			auto& rect = rects[i];
			const auto& it = regions_[i];
			rect.id = i;
			rect.w = it.first.width;
			rect.h = it.first.height;
		}

		stbrp_pack_rects(&c, rects.data(), rects.size());

		//create texture and upload it to opengl
		create(width, height, nullptr);

		float tx = 1.0f / width;
		float ty = 1.0f / height;

		for (const auto& rect : rects) {
			const auto& image = regions_[rect.id].first;
			auto& region = regions_[rect.id].second;

			region.dx = rect.x * tx;
			region.dy = rect.y * ty;
			region.dw = rect.w * tx;
			region.dh = rect.h * ty;

			glTexSubImage2D(type_, 0, rect.x, rect.y, rect.w, rect.h, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
		}
	}

	void GLTextureAtlas::clear() {
		destroy();
		for (auto& it : regions_) {
			if (it.first.data != nullptr) {
				stbi_image_free(it.first.data);
			}
		}
		regions_.clear();
	}

	constexpr TextureRegion emptyRegion{ 0.0f, 0.0f, 0.0f, 0.0f };

	const TextureRegion& GLTextureAtlas::getTextureRegion(const std::string &name) const {
		for (const auto& it : regions_) {
			if (it.first.name == name) { return it.second; }
		}
		return emptyRegion;
	}



}
