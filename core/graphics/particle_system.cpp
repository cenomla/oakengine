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

	void ParticleSystem::setMesh(uint32_t layer, const Material *material, const Mesh *mesh, const TextureRegion& region) {
		layer_ = layer;
		material_ = material;
		mesh_ = mesh;
		region_ = region;

		storage_->bind();
		storage_->instance(material->layout, mesh_->vertices.size() * sizeof(Mesh::Vertex));
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
		batch_.instances = 1000;
		batch_.layer = layer_;
		
		storage_->data(0, mesh_->vertices.size() * sizeof(Mesh::Vertex) + 1000 * sizeof(glm::vec3), nullptr);
		storage_->data(1, mesh_->indices.size() * sizeof(uint32_t), nullptr);

		void *data = storage_->map(0);
		void *idata = storage_->map(1);


		memcpy(data, mesh_->vertices.data(), mesh_->vertices.size() * sizeof(Mesh::Vertex));
		memcpy(idata, mesh_->indices.data(), mesh_->indices.size() * sizeof(uint32_t));
		//transform uvs
		Mesh::Vertex *vd;
		for (size_t i = 0; i < mesh_->vertices.size(); i++) {
			vd = static_cast<Mesh::Vertex*>(data);
			vd->uv *= region_.extent;
			vd->uv += region_.pos;
			data = static_cast<Mesh::Vertex*>(data) + 1;
		}

		//upload rest of data
		for (int i = 0; i < 1000; i++) {
			*static_cast<glm::vec3*>(data) = particles_[i].position;
			data = static_cast<glm::vec3*>(data) + 1;
		}

		storage_->unmap(0);
		storage_->unmap(1);
	}

}