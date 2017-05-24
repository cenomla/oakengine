#pragma once

#include <glm/glm.hpp>

#include "mesh.h"
#include "batch.h"
#include "material.h"

namespace oak::graphics {

	class BufferStorage;

	class StaticBatcher {
	public:

		StaticBatcher();

		void addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material);
		void removeMesh(const Mesh *mesh);

		void run(BufferStorage *storage, BufferStorage *istorage);
		
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct MeshInfo {
			glm::mat4 transform;
			const Mesh *mesh;
			const Material *material;

			bool operator<(const MeshInfo& rhs) const { return material < rhs.material; }
		};

		oak::vector<MeshInfo> meshes_;
		oak::vector<Batch> batches_;

		size_t size_, isize_, maxSize_, maxISize_;

		bool needsRebatch_;

	};

}