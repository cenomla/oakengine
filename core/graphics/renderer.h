#pragma once

namespace oak::graphics {

	class Api;
	struct Pipeline;

	class Renderer {
	public:
		virtual void render(Api *api) = 0;
		virtual void setPipeline(Pipeline *pipeline) = 0;
	};

}