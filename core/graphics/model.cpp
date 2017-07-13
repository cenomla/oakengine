#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture.h"

#include "log.h"

namespace oak::graphics {

	void Model::load(const oak::string& path) {

		Assimp::Importer importer;

		const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			log_print_err("failed to load model: %s, error: %s", path.c_str(), importer.GetErrorString());
			return;
		}

		processNode(scene, scene->mRootNode);

	}

	void Model::setTextureRegion(const TextureRegion& region) {
		for (auto& mesh: meshes_) {
			meshSetTextureRegion<Vertex>(&mesh, region);
		}
	}

	void Model::processNode(const aiScene *scene, aiNode *node) {
		//process all meshs
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			processMesh(scene, scene->mMeshes[node->mMeshes[i]]);
		}

		//process all children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(scene, node->mChildren[i]);
		}
	}

	void Model::processMesh(const aiScene *scene, aiMesh *mesh) {
		
		oak::vector<Vertex> vertices;
		oak::vector<uint32_t> indices;
		oak::vector<int> textures;
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;

			if (mesh->mTextureCoords[0]) {
				vertex.uv.x = mesh->mTextureCoords[0][i].x;
				vertex.uv.y = mesh->mTextureCoords[0][i].y;
			} else {
				vertex.uv = glm::vec2{ 0.0f };
			}

			vertices.push_back(vertex);
		}

		if (mesh->mMaterialIndex >= 0) {

		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		Mesh nMesh;
		nMesh.data.resize(vertices.size() * sizeof(Vertex) / 4);
		memcpy(nMesh.data.data(), vertices.data(), vertices.size() * sizeof(Vertex));
		nMesh.indices = indices;
		nMesh.vertexCount = vertices.size();

		meshes_.push_back(nMesh);
	}

}