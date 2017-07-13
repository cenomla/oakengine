#pragma once

#include "graphics/api.h"
#include "container.h"

namespace oak::graphics {

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
						s += 4 * sizeof(char);
						break;
					case AttributeType::POSITION: case AttributeType::NORMAL:
						s += 3 * sizeof(float);
					default: break;
				}
			}
			return s;
		}

		size_t instance_stride() const {
			size_t s = 0;
			for (const auto& type : attributes) {
				switch (type) {
					case AttributeType::INSTANCE_TRANSFORM:
						s += 16 * sizeof(float);
						break;
					case AttributeType::INSTANCE_POSITION: case AttributeType::INSTANCE_NORMAL:
						s += 3 * sizeof(float);
						break;
					case AttributeType::INSTANCE_POSITION2D: case AttributeType::INSTANCE_UV:
						s += 2 * sizeof(float);
						break;
					case AttributeType::INSTANCE_COLOR:
						s += 4 * sizeof(char);
						break;
					default: break;
				}
			}
			return s;
		}
	};

}