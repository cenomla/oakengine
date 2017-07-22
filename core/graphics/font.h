#pragma once

#include "sprite.h"
#include "container.h"

namespace oak::graphics {

	struct Font {
		struct Glyph {
			Sprite sprite;
			float advance;
			char id;
		};

		size_t size;
		oak::vector<Glyph> glyphs;
	};
	
	Font loadFont(const oak::string& path);
}