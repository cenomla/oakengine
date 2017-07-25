#pragma once

#include <glm/glm.hpp>
#include "buffer_storage.h"
#include "texture.h"
#include "batch.h"
#include "mesh.h"

#include <random>

namespace oak::graphics {

	class BufferStorage;

	class ParticleSystem {
	public:

		ParticleSystem();

		void init();
		void terminate();

		void setMesh(uint32_t layer, const Material *material, const Mesh *mesh, const TextureRegion& region);

		void run();

		inline const Batch& getBatch() const { return batch_; }

	private:
		struct Particle {
			glm::vec3 force{ 0.0f };
			glm::vec3 position{ 32.0f };
			int life = 0;
		};

		std::random_device rd_;
		std::mt19937 re_;

		uint32_t layer_;
		const Material *material_;
		const Mesh *mesh_;
		TextureRegion region_;
		Particle particles_[1000];
		Batch batch_;

		BufferStorage storage_;
		AttributeLayout layout_;
	};

}
