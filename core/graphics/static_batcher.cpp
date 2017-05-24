#include "static_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

namespace oak::graphics {


	StaticBatcher::StaticBatcher() : size_{ 0 }, maxSize_{ 0 } {}

	void StaticBatcher::addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material) {
		meshes_.push_back({ transform, mesh, material });
		size_ += mesh->getVertexCount() * mesh->getDescriptor().stride;
		needsRebatch_ = true;
	}

	void StaticBatcher::removeMesh(const Mesh *mesh) {
		meshes_.erase(std::remove_if(std::begin(meshes_), std::end(meshes_), [&mesh](const MeshInfo& info){ return info.mesh == mesh; }), std::end(meshes_));
		size_ -= mesh->getVertexCount() * mesh->getDescriptor().stride;
		needsRebatch_ = true;
	}

	void StaticBatcher::run(BufferStorage *storage) {
		if (meshes_.empty() || !needsRebatch_) { return; }
	
		batches_.clear();

		std::sort(std::begin(meshes_), std::end(meshes_));

		if (size_ > maxSize_) {
			maxSize_ = size_;
			storage->data(maxSize_, nullptr, 0);
		}

		//a batch is a range of object with the same material
		void *buffer = storage->map(0);
		size_t offset = 0;
		const Material *mat = meshes_[0].material;

		Batch currentBatch{ meshes_[0].mesh->getDescriptor(), mat, offset, 0 }; //first batch
		//iterate through the sorted object
		for (auto& it : meshes_) {
			//if the material is different use a different batch
			if (mat != it.material) {
				offset += currentBatch.count;
				batches_.push_back(currentBatch);
				mat = it.material;
				currentBatch = Batch{ it.mesh->getDescriptor(), mat, offset, 0 };
			}
			//stream data
			it.mesh->draw(buffer, it.transform);
			buffer = static_cast<char*>(buffer) + it.mesh->getVertexCount() * currentBatch.desc.stride;

			currentBatch.count += it.mesh->getVertexCount();
		}
		batches_.push_back(currentBatch);

		storage->unmap();
	
		needsRebatch_ = false;
	}

}