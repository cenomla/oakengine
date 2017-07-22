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

		void setBufferStorage(BufferStorage *storage);

		void addMesh(uint32_t layer, const Material *material, const Mesh *mesh, const glm::mat4& transform, const TextureRegion& region);
		void updateMesh(const Mesh *mesh, const glm::mat4& transform, const TextureRegion& region);
		void removeMesh(const Mesh *mesh);

		void update();
		void run();
		
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct BufferInfo {
			BufferStorage *storage = nullptr;
			size_t size[2]{ 0 }, capacity[2]{ 0 };
			size_t offset = 0, count = 0;
			void *map[2]{ nullptr };
		} bufferInfo_;

		struct MeshInfo {
			uint64_t layer;
			const Material *material;
			const Mesh *mesh;
			glm::mat4 transform;
			TextureRegion region;

			inline bool operator<(const MeshInfo& rhs) const { 
				return layer == rhs.layer ? 
					material < rhs.material : 
					layer < rhs.layer; 
			}
		};

		oak::vector<MeshInfo> meshes_;
		oak::vector<Batch> batches_;

		bool needsRebatch_;
	};

}