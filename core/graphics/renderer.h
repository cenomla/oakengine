#pragma once

#include <cstddef>

#include "batch.h"

namespace oak::graphics {

	class BufferStorage;

	class Renderer {
	public:
		virtual void init() = 0;
		virtual void render(const Batch& batch) = 0;
		virtual BufferStorage* getStorage() = 0;
		virtual BufferStorage* getIStorage() = 0;
	};

}