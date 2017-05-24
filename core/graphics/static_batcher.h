#pragma once

#include <glm/glm.hpp>

#include "mesh.h"
#include "batch.h"
#include "material.h"

namespace oak::graphics {

	class StaticBatcher {
	public:

		StaticBatcher();
		~StaticBatcher();

		void addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material);
		void removeMesh(const Mesh *mesh);

		void run();

		inline const void* getData() const { return vertexData_; }
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct MeshInfo {
			glm::mat4 transform;
			const Mesh *mesh;
			const Material *material;
			void *data;

			bool operator<(const MeshInfo& rhs) const { return material < rhs.material; }
		};

		oak::vector<MeshInfo> meshes_;
		oak::vector<Batch> batches_;

		void *vertexData_;
		void *indexData_;

		size_t vertexCount_;
		size_t size_;
		size_t maxSize_;

		bool needsRebatch_;

	};

}