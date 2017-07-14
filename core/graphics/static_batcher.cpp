#include "static_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

#include "log.h"

namespace oak::graphics {


	StaticBatcher::StaticBatcher() {}

	void StaticBatcher::addBufferStorage(BufferStorage *storage) {
		//add the buffer to the list and hash its layout for quick comparison
		buffers_.push_back(BufferInfo{ storage });
	}

	void StaticBatcher::addMesh(uint32_t layer, const Material *material, const Mesh *mesh, const glm::mat4& transform, const TextureRegion& region) {
		meshes_.push_back({ layer, material, mesh, transform, region, nullptr });
		needsRebatch_ = true;
	}

	void StaticBatcher::removeMesh(const Mesh *mesh) {
		meshes_.erase(std::remove_if(std::begin(meshes_), std::end(meshes_), [&mesh](const MeshInfo& info){ return info.mesh == mesh; }), std::end(meshes_));
		needsRebatch_ = true;
	}

	void StaticBatcher::update() {
		needsRebatch_ = true;
	}	

	void StaticBatcher::run() {
		if (meshes_.empty() || !needsRebatch_) { return; }
	
		batches_.clear();

		std::sort(std::begin(meshes_), std::end(meshes_));

		//create batches 
		const Material *mat = meshes_[0].material;
		uint32_t layer = meshes_[0].layer;
		BufferInfo *bl = findBuffer(mat->layout);
		Batch currentBatch{ bl->storage, mat, bl->offset, 0, layer }; //first batch
		//iterate through the sorted object
		for (auto& it : meshes_) {
			//if the material is different use a different batch
			if (mat != it.material || layer != it.layer) {
				//update mat and layer
				mat = it.material;
				layer = it.layer;
				//offset the buffer layout
				bl->offset += currentBatch.count;
				bl = findBuffer(mat->layout);
				//make a new batch
				batches_.push_back(currentBatch);
				currentBatch = Batch{ bl->storage, mat, bl->offset, 0, layer };
			}
			it.bl = bl;
			size_t ic = it.mesh->indices.size();
			currentBatch.count += ic;
			bl->size[0] += it.mesh->vertices.size() * sizeof(Mesh::Vertex); //add to size of buffer total size of mesh
			bl->size[1] += ic * sizeof(uint32_t); //add to size of index buffer total size of indices array
		}
		batches_.push_back(currentBatch);

		//resize buffers
		for (auto& it : buffers_) {
			for (int i = 0; i < 2; i++) {
				if (it.size[i] > it.capacity[i]) {
					it.capacity[i] = it.size[i];
					it.storage->data(i, it.capacity[i], nullptr);
				}
			}
		}

		//fill batches

		//map buffers
		for (auto& it : buffers_) {
			for (int i = 0; i < 2; i++) {
				if (it.size[i] > 0) {
					it.map[i] = it.storage->map(i);
				}
			}
		}

		//copy data to buffers
		for (auto& it : meshes_) {
			bl = it.bl;
			if (bl->map[0]) {
				Mesh::Vertex *vd;
				for (const auto& v : it.mesh->vertices) {
					vd = static_cast<Mesh::Vertex*>(bl->map[0]);
					vd->position = glm::vec3{ it.transform * glm::vec4{ v.position, 1.0f } };
					vd->normal = glm::vec3{ it.transform * glm::vec4{ v.normal, 0.0f } };
					vd->uv = v.uv * it.region.extent + it.region.pos;
					bl->map[0] = static_cast<Mesh::Vertex*>(bl->map[0]) + 1;
				}
			}
			if (bl->map[1]) {
				uint32_t *id;
				for (const auto& i : it.mesh->indices) {
					id = static_cast<uint32_t*>(bl->map[1]);
					*id = i + bl->count;
					bl->map[1] = static_cast<uint32_t*>(bl->map[1]) + 1;
				}
			}
			bl->count += it.mesh->vertices.size();
		}

		//unmap and reset buffers
		for (auto& it : buffers_) {
			for (int i = 0; i < 2; i++) {
				if (it.map[i]) {
					it.size[i] = 0;
					it.map[i] = nullptr;
					it.storage->unmap(i);
				}
			}
			it.offset = 0;
			it.count = 0;
		}
	
		needsRebatch_ = false;
	}

	StaticBatcher::BufferInfo* StaticBatcher::findBuffer(const AttributeLayout *layout) {
		for (auto& bl : buffers_) {
			if (bl.storage->getLayout() == layout) {
				return &bl;
			}
		}
		return nullptr;
	}

}