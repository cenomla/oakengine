#pragma once

#ifdef OAK_USE_VULKAN
	#include "graphics/vulkan/api.h"
#endif
#ifdef OAK_USE_GL
	#include "graphics/opengl/api.h"
#endif

#include "events.h"


namespace oak::graphics {

	template<class TApi>
	class Renderer {
	public:
		inline void init(GLFWwindow *window) { api_.init(window); }
		inline void destroy() { api_.destroy(); }

		inline void update() { api_.update(); }

		inline void onWindowResize(int width, int height) { api_.onWindowResize(width, height); }

		inline void operator()(const ResizeEvent &evt) { onWindowResize(evt.width, evt.height); }

	private:
		TApi api_;
	};

}

