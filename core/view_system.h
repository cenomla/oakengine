#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "graphics/opengl/gl_buffer.h"
#include "view.h"
#include "system.h"

namespace oak {

	class ViewSystem : public System {
	public:
		ViewSystem(Engine &engine);

		void defineView(size_t viewId, const std::vector<uint32_t>& layers);
		void setView(size_t viewId, const View& view);

		glm::vec2 transformPoint(size_t viewId, const glm::vec2& point);

		size_t getViewId(uint32_t layer) const;
		const View& getViewByLayer(uint32_t layer) const;
		const View& getView(size_t viewId) const;
	private:
		struct ViewBlock {
			View view;

			struct {
				glm::mat4 proj;
				glm::mat4 view;
				glm::mat4 model;
			} matrixBlock;

			graphics::GLBuffer ubo{ GL_UNIFORM_BUFFER };
		};

		std::vector<size_t> layerToView_;
		std::vector<ViewBlock> views_;
	};

}