#include "particle_system.h"

#include <glad/glad.h>
#include <cstring>

#include "material.h"
#include "mesh.h"

namespace oak::graphics {

	ParticleSystem::ParticleSystem() : re_{ rd_() } {

	}

	void ParticleSystem::setBufferStorage(BufferStorage *storage) {
		storage_ = storage;
	}

	void ParticleSystem::setMesh(const Mesh *mesh, const Material *material, uint32_t layer) {
		mesh_ = mesh;
		material_ = material;
		layer_ = layer;

		storage_->bind();
		storage_->instance(mesh_->data.size() * 4);
		storage_->unbind();
	}

	void ParticleSystem::run() {

		//simulate particles
		std::uniform_real_distribution<float> fdist{ -1.0f, 1.0f };
		std::uniform_real_distribution<float> pdist{ 0.0f, 64.0f };
		std::uniform_int_distribution<int> ldist{ 16, 128 };

		for (auto& part : particles_) {
			part.position += part.force;
			
			part.life--;
			if (part.life < 0) {
				part.position = glm::vec3{ pdist(re_), pdist(re_), pdist(re_) };
				part.force = glm::normalize(glm::vec3{ fdist(re_), fdist(re_), fdist(re_) });
				part.life = ldist(re_);
			}
		}


		//create the batch
		batch_.material = material_;
		batch_.storage = storage_;
		batch_.count = mesh_->indices.size();
		batch_.offset = 0;
		batch_.instances = 640;
		batch_.layer = layer_;
		
		storage_->data(0, mesh_->data.size() * 4 + 640 * 12, nullptr);
		storage_->data(1, mesh_->indices.size() * 4, nullptr);

		void *data = storage_->map(0);
		void *idata = storage_->map(1);

		memcpy(data, mesh_->data.data(), mesh_->data.size() * 4);
		memcpy(idata, mesh_->indices.data(), mesh_->indices.size() * 4);
		data = static_cast<float*>(data) + mesh_->data.size();

		//upload rest of data
		for (int i = 0; i < 640; i++) {
			*static_cast<glm::vec3*>(data) = particles_[i].position;
			data = static_cast<glm::vec3*>(data) + 1;
		}

		storage_->unmap(0);
		storage_->unmap(1);

	}

}