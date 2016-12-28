#pragma once

#include <vector>
#include <initializer_list>
#include <glm/glm.hpp>

#include "graphics/opengl/gl_buffer.h"
#include "system.h"

namespace oak {

	struct View {
		int x = 0, y = 0, width = 0, height = 0;
	};

	class ViewSystem : public System {
	public:
		ViewSystem(Engine &engine);

		void defineView(size_t viewId, std::initializer_list<uint32_t> &&layers);
		void setView(size_t viewId, const View &view);

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