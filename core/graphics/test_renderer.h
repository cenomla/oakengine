#pragma once

#include "static_batcher.h"
#include "renderer.h"
#include "system.h"
#include "entity_cache.h"

namespace oak {
	class Scene;
}

namespace oak::graphics {

	class TestRenderer : public System {
	public:
		TestRenderer(oak::Scene& scene);

		void init() override;
		void run() override;
		
		StaticBatcher batcher_;
	private:

		Renderer *renderer_;

		Scene *scene_;
		EntityCache cache_;



	};

}