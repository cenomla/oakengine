#pragma once

#include <graphics/buffer_storage.h>
#include <graphics/static_batcher.h>
#include <graphics/sprite_batcher.h>
#include <graphics/particle_system.h>
#include <system.h>
#include <entity_cache.h>

namespace oak {
	class Scene;
}

namespace oak::graphics {
	class Api;
	class Renderer;
}

class RenderSystem : public oak::System {
public:
	RenderSystem(oak::Scene *scene, oak::graphics::Api *api);

	void init() override;
	void terminate() override;
	void run() override;
	
private:
	oak::Scene *scene_;
	oak::EntityCache meshCache_;
	oak::EntityCache textCache_;

	oak::graphics::Api *api_;
	oak::vector<oak::graphics::Renderer*> layers_;
	oak::vector<oak::graphics::Batch> batches_;

	oak::graphics::SpriteBatcher batcher_;
	
	oak::graphics::BufferStorage storageMesh_;
	oak::graphics::Shader shader_;
	oak::graphics::Material material_;

	oak::graphics::Api::State state_;
};
