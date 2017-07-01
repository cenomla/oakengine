#pragma once

#include "container.h"

namespace oak::graphics {

	enum class AttributeType {
		POSITION,
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
					case AttributeType::UV:
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