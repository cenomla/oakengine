#include "static_batcher.h"

#include <algorithm>

namespace oak::graphics {


	StaticBatcher::StaticBatcher() : vertexData_{ nullptr }, size_{ 0 }, maxSize_{ 0 } {}

	StaticBatcher::~StaticBatcher() {
		if (vertexData_ != nullptr) {
			oak_allocator.deallocate(vertexData_, maxSize_);
			size_ = 0;
			maxSize_ = 0;
			vertexData_ = nullptr;
		}
	}

	void StaticBatcher::addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material) {
		meshes_.push_back({ transform, mesh, material, nullptr });
		vertexCount_ += mesh->getVertexCount();
		size_ += mesh->getVertexCount() * mesh->getDescriptor().stride;
		needsRebatch_ = true;
	}

	void StaticBatcher::removeMesh(const Mesh *mesh) {
		meshes_.erase(std::remove_if(std::begin(meshes_), std::end(meshes_), [&mesh](const MeshInfo& info){ return info.mesh == mesh; }), std::end(meshes_));
		vertexCount_ -= mesh->getVertexCount();
		size_ -= mesh->getVertexCount() * mesh->getDescriptor().stride;
		needsRebatch_ = true;
	}

	void StaticBatcher::run() {
		if (meshes_.empty() || !needsRebatch_) { return; }
	
		batches_.clear();

		std::sort(std::begin(meshes_), std::end(meshes_));


		if (size_ > maxSize_) {
			if (vertexData_ != nullptr) {
				oak_allocator.deallocate(vertexData_, maxSize_);
			}
			maxSize_ = size_;
			vertexData_ = oak_allocator.allocate(maxSize_);
		}

		//a batch is a range of object with the same material
		void *buffer = vertexData_;
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
			it.data = buffer;
			it.mesh->draw(buffer, it.transform);
			buffer = static_cast<char*>(buffer) + it.mesh->getVertexCount() * currentBatch.meshDesc.stride;

			currentBatch.count += it.mesh->getVertexCount();
		}
		batches_.push_back(currentBatch);
	
		needsRebatch_ = false;
	}

}