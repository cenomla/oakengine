#pragma once

#include "container.h"

namespace oak::graphics {

	struct Font {
		struct Glyph {
			float dx, dy, dw, dh, xoffset, yoffset, width, height, advance;
			char id;
		};

		size_t size;
		oak::vector<Glyph> glyphs;
	};
	
	Font loadFont(const oak::string& path);

}