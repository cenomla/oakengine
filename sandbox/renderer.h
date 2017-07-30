#pragma once

#include "pipeline.h"

namespace oak::graphics {
	class Api;
}

class Renderer {
public:
	virtual void render(oak::graphics::Api *api) = 0;
	virtual void setPipeline(const Pipeline *pipeline) = 0;
};