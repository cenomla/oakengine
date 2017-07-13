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
		void setMesh(const Mesh *mesh, const Material *material, uint32_t layer);

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

		const Mesh *mesh_;
		const Material *material_;
		uint32_t layer_;
		Particle particles_[6400];
		Batch batch_;

		BufferStorage *storage_;

		
	};

}