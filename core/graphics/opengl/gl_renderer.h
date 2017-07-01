#pragma once

#include "graphics/renderer.h"


struct GLFWwindow;

namespace oak::graphics {

	class GLRenderer : public Renderer {
	public:

		void init() override;
		void terminate() override;
		void render() override;
		
		inline void setPipeline(Pipeline *pipeline) override { pipeline_ = pipeline; }

	private:
		GLFWwindow *window_;
		Pipeline *pipeline_;
	};

}