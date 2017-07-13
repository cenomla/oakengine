#pragma once

#include <glm/glm.hpp>

#include "mesh.h"
#include "texture.h"
#include "container.h"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace oak::graphics {

	class Model {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		void load(const oak::string& path);
		void setTextureRegion(const TextureRegion& region);

		const oak::vector<Mesh>& getMeshes() const { return meshes_; }

	private:

		void processNode(const aiScene *scene, aiNode *node);
		void processMesh(const aiScene *scene, aiMesh *mesh);

		oak::vector<Mesh> meshes_;
	};

}