#pragma once

#include "static_batcher.h"
#include "renderer.h"
#include "system.h"
#include "entity_cache.h"

namespace oak {
	class Scene;
}

namespace oak::graphics {

	class RenderSystem : public System {
	public:
		RenderSystem(Scene& scene, Renderer& renderer);

		void init() override;
		void terminate() override;
		void run() override;
		
		StaticBatcher batcher_;
	private:
		Scene *scene_;
		EntityCache cache_;
		Renderer *renderer_;
	};

}