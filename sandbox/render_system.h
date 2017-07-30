#pragma once

#include <graphics/buffer_storage.h>
#include <graphics/static_batcher.h>
#include <graphics/sprite_batcher.h>
#include <graphics/particle_system.h>
#include <system.h>
#include <entity_cache.h>

#include "pipeline.h"

namespace oak {
	class Scene;
}

namespace oak::graphics {
	class Api;
}

class Renderer;

class RenderSystem : public oak::System {
public:
	RenderSystem(oak::Scene *scene, oak::graphics::Api *api);

	void pushLayerFront(Renderer *renderer);
	void pushLayerBack(Renderer *renderer);

	void removeLayer(Renderer& renderer);

	void init() override;
	void terminate() override;
	void run() override;
	
private:
	oak::Scene *scene_;
	oak::EntityCache meshCache_;
	oak::EntityCache spriteCache_;
	oak::EntityCache textCache_;
	oak::EntityCache particleCache_;

	oak::graphics::Api *api_;
	Pipeline pipeline_;
	oak::vector<Renderer*> layers_;

	oak::graphics::StaticBatcher meshBatcher_;
	oak::graphics::SpriteBatcher spriteBatcher_;
	oak::graphics::ParticleSystem particleSystem_;
};
