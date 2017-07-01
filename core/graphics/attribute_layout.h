#pragma once

#include "container.h"

namespace oak::graphics {

	enum class AttributeType {
		POSITION,
		POSITION2D, 
		NORMAL,
		UV,
		COLOR
	};

	struct AttributeLayout {
		oak::vector<AttributeType> attributes;

		size_t stride() const {
			size_t s = 0;
			for (const auto& type : attributes) {
				switch (type) {
					case AttributeType::UV: case AttributeType::POSITION2D:
						s += 2 * sizeof(float);
						break;
					case AttributeType::COLOR:
						s += 3;
						break;
					default:
						s += 3 * sizeof(float);
				}
			}
			return s;
		}
	};

}

namespace std {

	template<>
	struct hash<oak::graphics::AttributeLayout> {
		size_t operator()(const oak::graphics::AttributeLayout& layout) const { //sdbm hash function
			size_t h = 0;
			for (const auto& type : layout.attributes) {
				h = static_cast<int>(type) + (h << 6) + (h << 16) - h;
			}
			return h;
		}
	};

}