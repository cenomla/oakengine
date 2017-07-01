#include "static_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

#include "log.h"

namespace oak::graphics {


	StaticBatcher::StaticBatcher() {}

	void StaticBatcher::addBufferStorage(const AttributeLayout *layout, BufferStorage *storage) {
		//add the buffer to the list and hash its layout for quick comparison
		buffers_.push_back(BufferLayout{ layout, storage, std::hash<AttributeLayout>{}(*layout), { 0, 0 }, { 0, 0 }, 0, 0, { nullptr, nullptr } });
	}

	void StaticBatcher::addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material) {
		if (std::hash<AttributeLayout>{}(mesh->getLayout()) == std::hash<AttributeLayout>{}(*material->layout)) {
			meshes_.push_back({ transform, mesh, material });
			needsRebatch_ = true;
		} else {
			log_print_warn("model::mesh attribute mismatch");
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
		BufferLayout *bl = findBuffer(std::hash<AttributeLayout>{}(*mat->layout));
		Batch currentBatch{ bl->storage, mat, bl->offset, 0 }; //first batch
		//iterate through the sorted object
		for (auto& it : meshes_) {
			//if the material is different use a different batch
			if (mat != it.material) {
				bl->offset += currentBatch.count;
				batches_.push_back(currentBatch);
				mat = it.material;
				bl = findBuffer(std::hash<AttributeLayout>{}(*mat->layout));
				currentBatch = Batch{ bl->storage, mat, bl->offset, 0 };
			}
			it.bl = bl;
			currentBatch.count += it.mesh->getIndexCount();
			bl->size[0] += it.mesh->getVertexCount() * it.mesh->getLayout().stride(); //add to size of buffer total size of model
			bl->size[1] += it.mesh->getIndexCount() * 4; //add to size of index buffer total size of indices array
		}
		batches_.push_back(currentBatch);

		//resize buffers
		for (auto& it : buffers_) {
			for (int i = 0; i < 2; i++) {
				if (it.size[i] > it.capacity[i]) {
					it.capacity[i] = it.size[i];
					it.storage->data(i, it.capacity[i], nullptr, 0);
				}
			}
		}

		//fill batches

		//map buffers
		for (auto& it : buffers_) {
			if (it.size[0] > 0) {
				it.map[0] = it.storage->map(0, 0);
				it.map[1] = it.storage->map(1, 0);
			}
		}

		//copy data to buffers
		for (auto& it : meshes_) {
			bl = it.bl;
			it.mesh->draw(bl->map[0], bl->map[1], it.transform, bl->count);
			bl->count += it.mesh->getVertexCount();
			bl->map[0] = static_cast<char*>(bl->map[0]) + it.mesh->getVertexCount() * it.mesh->getLayout().stride();
			bl->map[1] = static_cast<int*>(bl->map[1]) + it.mesh->getIndexCount();
		}
		batches_.push_back(currentBatch);

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

	StaticBatcher::BufferLayout* StaticBatcher::findBuffer(size_t hash) {
		for (auto& bl : buffers_) {
			if (bl.hash == hash) {
				return &bl;
			}
		}
		return nullptr;
	}

}