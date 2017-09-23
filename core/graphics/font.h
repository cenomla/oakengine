#pragma once

#include "container.h"
#include "sprite.h"
#include "resource.h"

namespace oak::graphics {

	struct Font {
		static const TypeInfo typeInfo;

		struct Glyph {
			Sprite sprite;
			float advance;
			char id;
		};

		size_t size;
		oak::vector<Glyph> glyphs;
	};

	void pup(Puper& puper, Font& data, const ObjInfo& info);
	
	Font loadFont(const oak::string& path);
}
