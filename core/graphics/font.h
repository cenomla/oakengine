#pragma once

#include <string>
#include <vector>

namespace oak::graphics {

	class Font {
	public:
		Font(size_t mat);	

		void draw(void *buffer, const std::string &text, float x, float y, float rotation, float size) const;
	
		void create(const std::string &path);

		size_t getMaterialId() const { return materialId_; }
	private:
		struct Glyph {
			char id;
			float dx, dy, dw, dh, xoffset, yoffset, width, height, advance;	
		};

		size_t materialId_;
		size_t fontSize_;
		std::vector<Glyph> glyphs_;
	};

}