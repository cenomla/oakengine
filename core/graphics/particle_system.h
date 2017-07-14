#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "batch.h"

#include <random>

namespace oak::graphics {

	class BufferStorage;

	class ParticleSystem {
	public:

		ParticleSystem();

		void setBufferStorage(BufferStorage *storage);
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
		Particle particles_[640];
		Batch batch_;

		BufferStorage *storage_;

		
	};

}