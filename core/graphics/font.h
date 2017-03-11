#pragma once

#include "container.h"

namespace oak::graphics {

	class Font {
	public:
		Font(size_t mat);	

		void draw(void *buffer, const oak::string &text, float x, float y, float rotation, float size) const;
	
		void create(const oak::string &path);

		size_t getMaterialId() const { return materialId_; }
	private:
		struct Glyph {
			char id;
			float dx, dy, dw, dh, xoffset, yoffset, width, height, advance;	
		};

		size_t materialId_;
		size_t fontSize_;
		oak::vector<Glyph> glyphs_;
	};

}