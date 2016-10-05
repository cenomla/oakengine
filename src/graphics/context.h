#pragma once

#ifdef OAK_USE_VULKAN
	#include "graphics/vulkan/api.h"
#endif
#ifdef OAK_USE_GL
	#include "graphics/opengl/api.h"
#endif

#include "system.h"
#include "engine.h"
#include "events.h"


namespace oak::graphics {

	template<typename TApi>
	class Context : public System{
	public:
		typedef TApi type;

		Context(Engine *engine) : System{ engine } {}

		inline void init() override {
			engine_->getTaskManager().addTask( Task{ [this](){ update(); }, Task::LOOP_BIT });
		}
		inline void destroy() override { api_.destroy(); }

		inline void update() { api_.update(); }

		inline void operator()(const WindowCreateEvent &evt) { api_.init(evt.window); }
		inline void operator()(const ResizeEvent &evt) { api_.onWindowResize(evt.width, evt.height); }

	private:
		TApi api_;
	};

}

