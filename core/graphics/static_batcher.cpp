#include "static_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

#include "log.h"

namespace oak::graphics {


	StaticBatcher::StaticBatcher() {}

	void StaticBatcher::addBufferStorage(BufferStorage *storage) {
		//add the buffer to the list and hash its layout for quick comparison
		buffers_.push_back(BufferInfo{ storage, { 0, 0 }, { 0, 0 }, 0, 0, { nullptr, nullptr } });
	}

	void StaticBatcher::addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material, uint32_t layer) {
		meshes_.push_back({ layer, material, mesh, transform, nullptr });
		needsRebatch_ = true;
	}

	void StaticBatcher::updateMesh(const glm::mat4& transform, const Mesh *mesh) {
		for (auto& it : meshes_) {
			if (it.mesh == mesh) {
				it.transform = transform;
				needsRebatch_ = true;
				break;
			}
		}
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
			currentBatch.count += it.mesh->getIndexCount();
			bl->size[0] += it.mesh->getVertexCount() * mat->layout->stride(); //add to size of buffer total size of mesh
			bl->size[1] += it.mesh->getIndexCount() * 4; //add to size of index buffer total size of indices array
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
			if (it.size[0] > 0) {
				for (int i = 0; i < 2; i++) {
					it.map[i] = it.storage->map(i);
				}
			}
		}

		//copy data to buffers
		for (auto& it : meshes_) {
			bl = it.bl;
			it.mesh->draw(bl->map[0], bl->map[1], it.transform, bl->count);
			bl->count += it.mesh->getVertexCount();
			bl->map[0] = static_cast<char*>(bl->map[0]) + it.mesh->getVertexCount() * mat->layout->stride();
			bl->map[1] = static_cast<int*>(bl->map[1]) + it.mesh->getIndexCount();
		}

		//unmap and reset buffers
		for (auto& it : buffers_) {
			if (it.size[0] > 0) {
				for (int i = 0; i < 2; i++) {
					it.size[i] = 0;
					it.map[i] = nullptr;
					it.storage->unmap(i);
				}
				it.offset = 0;
				it.count = 0;
			}
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