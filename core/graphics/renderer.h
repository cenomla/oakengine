#pragma once

#include <cstddef>

#include "batch.h"

namespace oak::graphics {

	class BufferStorage;

	class Renderer {
	public:
		virtual void init() = 0;
		virtual void render(BufferStorage *data, const Batch& batch) = 0;
	};

}