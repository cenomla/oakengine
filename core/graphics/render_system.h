#pragma once

#include "static_batcher.h"
#include "particle_system.h"
#include "system.h"
#include "entity_cache.h"
#include "pipeline.h"

namespace oak {
	class Scene;
}

namespace oak::graphics {

	class Api;
	class Renderer;

	class RenderSystem : public System {
	public:
		RenderSystem(Scene& scene, Api& api);

		void pushLayerFront(Renderer& renderer);
		void pushLayerBack(Renderer& renderer);

		void removeLayer(Renderer& renderer);

		void init() override;
		void terminate() override;
		void run() override;
		
		StaticBatcher batcher_;
		ParticleSystem particleSystem_;
	private:
		Scene *scene_;
		EntityCache cache_;

		Api *api_;
		Pipeline pipeline_;
		oak::vector<Renderer*> layers_;
		oak::vector<Batch> batches_;
	};

}