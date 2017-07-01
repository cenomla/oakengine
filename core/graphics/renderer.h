#pragma once

#include <cstddef>

#include "pipeline.h"
#include "batch.h"

namespace oak::graphics {

	class Renderer {
	public:
		virtual void init() = 0;
		virtual void terminate() = 0;
		virtual void setPipeline(Pipeline *pipeline) = 0;
		virtual void render() = 0;
	};

}