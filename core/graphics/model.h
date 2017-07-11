#pragma once

#include "mesh.h"

#include "container.h"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace oak::graphics {

	class Model {
	public:

		void load(const oak::string& path);

		const oak::vector<Mesh>& getMeshes() const { return meshes_; }

	private:

		void processNode(const aiScene *scene, aiNode *node);
		void processMesh(const aiScene *scene, aiMesh *mesh);

		oak::vector<Mesh> meshes_;
	};

}