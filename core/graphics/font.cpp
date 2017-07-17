#include "font.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "util/file_util.h"

namespace oak::graphics {

	struct FontHeaderInfo {
		size_t size, width, height;
	};

	Font loadFont(const oak::string& path) {
		const oak::string file = util::readFileAsString(path.c_str());
		size_t pos = 0, len = 0;

		Font font;
		FontHeaderInfo fhi;
		//parse header

		while((len = file.find(' ', pos)) != oak::string::npos) {
			const oak::string &token = file.substr(pos, len - pos);
			
			if (token.compare(0, 4, "size") == 0) {
				fhi.size = std::stoull(token.substr(5).c_str());
				font.size = fhi.size;
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
				Font::Glyph glyph;

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
				if (static_cast<size_t>(glyph.id) >= font.glyphs.size()) {
					font.glyphs.resize(static_cast<size_t>(glyph.id)+1);
				}
				font.glyphs[static_cast<size_t>(glyph.id)] = glyph;
			}

			pos = len + 1;
		}

		return font;
	}

}
