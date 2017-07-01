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

		void addBufferStorage(const AttributeLayout *layout, BufferStorage *storage);

		void addMesh(const glm::mat4& transform, const Mesh *mesh, const Material *material);
		void removeMesh(const Mesh *mesh);

		void run();
		
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct BufferLayout {
			const AttributeLayout *layout;
			BufferStorage *storage;
			size_t hash;
			size_t size[2], capacity[2];
			size_t offset, count;
			void *map[2];

			bool operator==(const BufferLayout& other) const { return hash == other.hash; }
		};

		struct MeshInfo {
			glm::mat4 transform;
			const Mesh *mesh;
			const Material *material;
			BufferLayout *bl;

			bool operator<(const MeshInfo& rhs) const { return material < rhs.material; }
		};

		BufferLayout* findBuffer(size_t hash);

		oak::vector<MeshInfo> meshes_;
		oak::vector<Batch> batches_;
		oak::vector<BufferLayout> buffers_;

		bool needsRebatch_;

	};

}