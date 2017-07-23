#include "font.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "file_manager.h"

namespace oak::graphics {

	struct FontHeaderInfo {
		size_t size, width, height;
	};

	Font loadFont(const oak::string& path) {
		auto file = FileManager::inst().openFile(path);
		const oak::string glyphInfo = file.read<oak::string>();
		FileManager::inst().closeFile(file);
		size_t pos = 0, len = 0;

		Font font;
		FontHeaderInfo fhi;
		//parse header

		while((len = glyphInfo.find(' ', pos)) != oak::string::npos) {
			const oak::string &token = glyphInfo.substr(pos, len - pos);
			
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
		while((len = glyphInfo.find('\n', pos)) != oak::string::npos) {
			const oak::string &token = glyphInfo.substr(pos, len - pos);
			
			if (token.compare(0, 4, "char") == 0) {
				Font::Glyph glyph;

				glyph.id = std::stoi(token.substr(8, 7).c_str());
				glyph.sprite.region.pos.x = std::stof(token.substr(18, 4).c_str()) * (1.0f / fhi.width);
				glyph.sprite.region.pos.y = std::stof(token.substr(25, 4).c_str()) * (1.0f / fhi.height);
				glyph.sprite.region.extent.x = std::stof(token.substr(36, 4).c_str()) * (1.0f / fhi.width);
				glyph.sprite.region.extent.y = std::stof(token.substr(48, 4).c_str()) * (1.0f / fhi.height);
				glyph.sprite.centerX = -std::stof(token.substr(61, 4).c_str());
				glyph.sprite.centerY = -std::stof(token.substr(74, 4).c_str());
				glyph.sprite.width = std::stof(token.substr(36, 4).c_str());
				glyph.sprite.height = std::stof(token.substr(48, 4).c_str());
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
