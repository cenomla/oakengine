#include "font.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "util/file_util.h"
#include "sprite.h"

namespace oak::graphics {

	Font::Font(size_t mat) : materialId_{ mat }, fontSize_{ 0 } {

	}

	void Font::draw(void *buffer, const std::string &text, float x, float y, float rotation, float size) const {
		
		float scale = size / static_cast<float>(fontSize_);

		for (const char c : text) {
			const Glyph& g = glyphs_[static_cast<size_t>(c)];
			
			glm::vec2 pos{ x, y };
			static_cast<Sprite::Vertex*>(buffer)[0] = { glm::rotate(glm::vec2{ g.xoffset, g.yoffset } * scale, rotation) + pos, glm::vec2{ g.dx, g.dy } };
			static_cast<Sprite::Vertex*>(buffer)[1] = { glm::rotate(glm::vec2{ g.xoffset + g.width, g.yoffset } * scale, rotation) + pos, glm::vec2{ g.dx + g.dw, g.dy } };
			static_cast<Sprite::Vertex*>(buffer)[2] = { glm::rotate(glm::vec2{ g.xoffset + g.width, g.yoffset + g.height } * scale, rotation) + pos, glm::vec2{ g.dx + g.dw, g.dy + g.dh } };
			static_cast<Sprite::Vertex*>(buffer)[3] = { glm::rotate(glm::vec2{ g.xoffset, g.yoffset + g.height } * scale, rotation) + pos, glm::vec2{ g.dx, g.dy + g.dh } };

			buffer = static_cast<Sprite::Vertex*>(buffer) + 4;
			x += g.advance * scale;
		}
	}

	struct FontHeaderInfo {
		size_t size, width, height;
	};

	void Font::create(const std::string &path) {
		const std::string file = util::readFileAsString(path);
		size_t pos = 0, len = 0;

		FontHeaderInfo fhi;
		//parse header

		while((len = file.find(' ', pos)) != std::string::npos) {
			const std::string &token = file.substr(pos, len - pos);
			
			if (token.compare(0, 4, "size") == 0) {
				fhi.size = std::stoull(token.substr(5));
				fontSize_ = fhi.size;
			} else if (token.compare(0, 6, "scaleW") == 0) {
				fhi.width = std::stoull(token.substr(7));
			} else if (token.compare(0, 6, "scaleH") == 0) {
				fhi.height = std::stoull(token.substr(7));
			} else if (token.compare(0, 5, "count") == 0) {
				break;
			}

			pos = len + 1;
		}

		//parse glyphs
		while((len = file.find('\n', pos)) != std::string::npos) {
			const std::string &token = file.substr(pos, len - pos);
			
			if (token.compare(0, 4, "char") == 0) {
				Glyph glyph;

				glyph.id = std::stoi(token.substr(8, 7));
				glyph.dx = std::stof(token.substr(18, 4)) * (1.0f / fhi.width);
				glyph.dy = std::stof(token.substr(25, 4)) * (1.0f / fhi.height);
				glyph.dw = std::stof(token.substr(36, 4)) * (1.0f / fhi.width);
				glyph.dh = std::stof(token.substr(48, 4)) * (1.0f / fhi.height);
				glyph.xoffset = std::stof(token.substr(61, 4));
				glyph.yoffset = std::stof(token.substr(74, 4));
				glyph.width = std::stof(token.substr(36, 4));
				glyph.height = std::stof(token.substr(48, 4));
				glyph.advance = std::stof(token.substr(88, 4));
				//ensure size
				if (static_cast<size_t>(glyph.id) >= glyphs_.size()) {
					glyphs_.resize(static_cast<size_t>(glyph.id)+1);
				}
				glyphs_[static_cast<size_t>(glyph.id)] = glyph;
			}

			pos = len + 1;
		}

	}

}
