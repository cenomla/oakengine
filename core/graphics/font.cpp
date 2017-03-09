#include "font.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "util/file_util.h"
#include "sprite.h"

namespace oak::graphics {

	Font::Font(size_t mat) : materialId_{ mat }, fontSize_{ 0 } {

	}

	void Font::draw(void *buffer, const oak::string &text, float x, float y, float rotation, float size) const {
		
		const float scale = size / static_cast<float>(fontSize_);
		const float sx = x;

		for (const char c : text) {
			const Glyph& g = glyphs_[static_cast<size_t>(c)];
			
			glm::vec2 pos{ x, y };
			static_cast<Sprite::Vertex*>(buffer)[0] = { glm::rotate(glm::vec2{ g.xoffset, g.yoffset } * scale, rotation) + pos, glm::vec2{ g.dx, g.dy } };
			static_cast<Sprite::Vertex*>(buffer)[1] = { glm::rotate(glm::vec2{ g.xoffset + g.width, g.yoffset } * scale, rotation) + pos, glm::vec2{ g.dx + g.dw, g.dy } };
			static_cast<Sprite::Vertex*>(buffer)[2] = { glm::rotate(glm::vec2{ g.xoffset + g.width, g.yoffset + g.height } * scale, rotation) + pos, glm::vec2{ g.dx + g.dw, g.dy + g.dh } };
			static_cast<Sprite::Vertex*>(buffer)[3] = { glm::rotate(glm::vec2{ g.xoffset, g.yoffset + g.height } * scale, rotation) + pos, glm::vec2{ g.dx, g.dy + g.dh } };

			buffer = static_cast<Sprite::Vertex*>(buffer) + 4;
			x += g.advance * scale;
			if (c == '\n') {
				x = sx;
				y += size * 1.2f;
			}
		}
	}

	struct FontHeaderInfo {
		size_t size, width, height;
	};

	void Font::create(const oak::string &path) {
		const oak::string file = util::readFileAsString(path);
		size_t pos = 0, len = 0;

		FontHeaderInfo fhi;
		//parse header

		while((len = file.find(' ', pos)) != oak::string::npos) {
			const oak::string &token = file.substr(pos, len - pos);
			
			if (token.compare(0, 4, "size") == 0) {
				fhi.size = std::stoull(token.substr(5).c_str());
				fontSize_ = fhi.size;
			} else if (token.compare(0, 6, "scaleW") == 0) {
				fhi.width = std::stoull(token.substr(7).c_str());
			} else if (token.compare(0, 6, "scaleH") == 0) {
				fhi.height = std::stoull(token.substr(7).c_str());
			} else if (token.compare(0, 5, "count") == 0) {
				break;
			}

			pos = len + 1;
		}

		//parse glyphs
		while((len = file.find('\n', pos)) != oak::string::npos) {
			const oak::string &token = file.substr(pos, len - pos);
			
			if (token.compare(0, 4, "char") == 0) {
				Glyph glyph;

				glyph.id = std::stoi(token.substr(8, 7).c_str());
				glyph.dx = std::stof(token.substr(18, 4).c_str()) * (1.0f / fhi.width);
				glyph.dy = std::stof(token.substr(25, 4).c_str()) * (1.0f / fhi.height);
				glyph.dw = std::stof(token.substr(36, 4).c_str()) * (1.0f / fhi.width);
				glyph.dh = std::stof(token.substr(48, 4).c_str()) * (1.0f / fhi.height);
				glyph.xoffset = std::stof(token.substr(61, 4).c_str());
				glyph.yoffset = std::stof(token.substr(74, 4).c_str());
				glyph.width = std::stof(token.substr(36, 4).c_str());
				glyph.height = std::stof(token.substr(48, 4).c_str());
				glyph.advance = std::stof(token.substr(88, 4).c_str());
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
