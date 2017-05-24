#pragma once

#include <cstddef>

#include "batch.h"

namespace oak::graphics {

	class Renderer {
	public:
		virtual void init() = 0;
		virtual void render(const void *data, const Batch& batch) = 0;
	};

}