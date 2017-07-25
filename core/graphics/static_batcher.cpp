#include "static_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

#include "log.h"

namespace oak::graphics {

	void StaticBatcher::init() {
		//create the buffer storage
		bufferInfo_.storage.create({
			oak::vector<AttributeType> {
				AttributeType::POSITION,
				AttributeType::NORMAL,
				AttributeType::UV
			}	
		});
	}

	void StaticBatcher::terminate() {
		bufferInfo_.storage.destroy();
	}

	void StaticBatcher::addMesh(uint32_t layer, const Material *material, const Mesh *mesh, const glm::mat4& transform, const TextureRegion& region) {
		meshes_.push_back({ layer, material, mesh, transform, region });
		needsRebatch_ = true;
	}

	void StaticBatcher::updateMesh(const Mesh *mesh, const glm::mat4& transform, const TextureRegion& region) {
		for (auto& mi : meshes_) {
			if (mi.mesh == mesh) {
				mi.transform = transform;
				mi.region = region;
			}
		}
		needsRebatch_ = true;
	}

	void StaticBatcher::removeMesh(const Mesh *mesh) {
		meshes_.erase(std::remove_if(std::begin(meshes_), std::end(meshes_), [&mesh](const MeshInfo& info){ return info.mesh == mesh; }), std::end(meshes_));
		needsRebatch_ = true;
	}

	void StaticBatcher::run() {
		if (meshes_.empty() || !needsRebatch_) { return; }
	
		batches_.clear();

		std::sort(std::begin(meshes_), std::end(meshes_));

		//create batches 
		const Material *mat = meshes_[0].material;
		uint32_t layer = meshes_[0].layer;
		Batch currentBatch{ &bufferInfo_.storage, mat, bufferInfo_.offset, 0, layer }; //first batch
		//iterate through the sorted object
		for (auto& it : meshes_) {
			//if the material is different use a different batch
			if (mat != it.material || layer != it.layer) {
				//update mat and layer
				mat = it.material;
				layer = it.layer;
				//offset the buffer layout
				bufferInfo_.offset += currentBatch.count;
				//make a new batch
				batches_.push_back(currentBatch);
				currentBatch = Batch{ &bufferInfo_.storage, mat, bufferInfo_.offset, 0, layer };
			}
			size_t ic = it.mesh->indices.size();
			currentBatch.count += ic;
			bufferInfo_.size[0] += it.mesh->vertices.size() * sizeof(Mesh::Vertex); //add to size of buffer total size of mesh
			bufferInfo_.size[1] += ic * sizeof(uint32_t); //add to size of index buffer total size of indices array
		}
		batches_.push_back(currentBatch);

		//resize buffer
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.size[i] > bufferInfo_.capacity[i]) {
				bufferInfo_.capacity[i] = bufferInfo_.size[i];
				bufferInfo_.storage.data(i, bufferInfo_.capacity[i], nullptr);
			}
		}

		//fill batches
		//map buffers
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.size[i] > 0) {
				bufferInfo_.map[i] = bufferInfo_.storage.map(i);
			}
		}

		//copy data to buffers
		for (auto& it : meshes_) {
			if (bufferInfo_.map[0]) {
				Mesh::Vertex *vd;
				for (const auto& v : it.mesh->vertices) {
					vd = static_cast<Mesh::Vertex*>(bufferInfo_.map[0]);
					vd->position = glm::vec3{ it.transform * glm::vec4{ v.position, 1.0f } };
					vd->normal = glm::vec3{ it.transform * glm::vec4{ v.normal, 0.0f } };
					vd->uv = v.uv * it.region.extent + it.region.pos;
					bufferInfo_.map[0] = static_cast<Mesh::Vertex*>(bufferInfo_.map[0]) + 1;
				}
			}
			if (bufferInfo_.map[1]) {
				uint32_t *id;
				for (const auto& i : it.mesh->indices) {
					id = static_cast<uint32_t*>(bufferInfo_.map[1]);
					*id = i + bufferInfo_.count;
					bufferInfo_.map[1] = static_cast<uint32_t*>(bufferInfo_.map[1]) + 1;
				}
			}
			bufferInfo_.count += it.mesh->vertices.size();
		}

		//unmap and reset buffers
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.map[i]) {
				bufferInfo_.size[i] = 0;
				bufferInfo_.map[i] = nullptr;
				bufferInfo_.storage.unmap(i);
			}
		}
		bufferInfo_.offset = 0;
		bufferInfo_.count = 0;
	
		needsRebatch_ = false;
	}

}