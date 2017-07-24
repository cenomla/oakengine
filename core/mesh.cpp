#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "file_manager.h"
#include "mesh.h"
#include "log.h"

namespace oak::graphics {

	static void processMesh(oak::vector<Mesh>& meshes, const aiScene *scene, aiMesh *mesh) {
		
		oak::vector<Mesh::Vertex> vertices;
		oak::vector<uint32_t> indices;
		oak::vector<int> textures;
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Mesh::Vertex vertex;
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

		meshes.push_back({ vertices, indices });
	}

	static void processNode(oak::vector<Mesh>& meshes, const aiScene *scene, aiNode *node) {
		//process all meshs
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			processMesh(meshes, scene, scene->mMeshes[node->mMeshes[i]]);
		}

		//process all children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(meshes, scene, node->mChildren[i]);
		}
	}

	oak::vector<Mesh> loadModel(const oak::string& path) {

		Assimp::Importer importer;

		const aiScene *scene = importer.ReadFile(FileManager::inst().resolvePath(path).c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			log_print_err("failed to load model: %s, error: %s", path.c_str(), importer.GetErrorString());
			return {};
		}

		oak::vector<Mesh> meshes;
		meshes.reserve(scene->mNumMeshes);

		processNode(meshes, scene, scene->mRootNode);

		return meshes;
	}

	static glm::vec3 findSupport(const glm::vec3& dir, const Mesh& A, const Mesh& B) {
		float max = -1.0f;
		glm::vec3 sA = -dir, sB = dir;
		float d;
		//find support point in mesh a
		for (const auto& v : A.vertices) {
			d = glm::dot(v.position, dir);
			if (d > max) {
				max = d;
				sA = v.position;
			}
		}
		max = -1.0f;
		//find support point in mesh b
		for (const auto& v : B.vertices) {
			d = glm::dot(v.position, -dir);
			if (d > max) {
				max = d;
				sB = v.position;
			}
		}

		return sA - sB;
	}

	struct Simplex {
		glm::vec3 points[4];
		int c = 0;
	};

	static bool processSimplexLine(Simplex& splex, glm::vec3& D) {
		oak_assert(splex.c == 2);
		glm::vec3 AB = splex.points[1] - splex.points[0];
		glm::vec3 AO = -splex.points[1];
		if (glm::dot(AB, AO) > 0.0f) {
			//set direction vector to a vector that is perpindicular to the AB plane and points towards the origin
			D = glm::cross(glm::cross(AB, AO), AB);
			return false;
		}
		splex.points[0] = splex.points[1];
		splex.c = 1;
		D = AO;
		return false;
	}

	static bool processSimplexTriangle(Simplex& splex, glm::vec3& D) {
		oak_assert(splex.c == 3);
		glm::vec3 AB = splex.points[2] - splex.points[1];
		glm::vec3 AC = splex.points[2] - splex.points[0];
		glm::vec3 ABC = glm::cross(AB, AC);
		glm::vec3 AO = -splex.points[2];

		if (glm::dot(glm::cross(ABC, AC), AO) > 0.0f) {
			if (glm::dot(AC, AO) > 0.0f) {
				splex.points[1] = splex.points[2];
				splex.c = 2;
				D = glm::cross(glm::cross(AC, AO), AC);
			} else {
				goto star;
			}
		} else {
			if (glm::dot(glm::cross(AB, ABC), AO) > 0.0f) {
				goto star;
			} else {
				if (glm::dot(ABC, AO) > 0.0f) {
					D = ABC;
				} else {
					auto& t = splex.points[1];
					splex.points[1] = splex.points[0];
					splex.points[0] = t;
					D = -ABC;
				}
			}
		}
		return false;

		star: 
		if (glm::dot(AB, AO) > 0.0f) {
			splex.points[0] = splex.points[1];
			splex.points[1] = splex.points[2];
			splex.c = 2;
			D = glm::cross(glm::cross(AB, AO), AB);
		} else {
			splex.points[0] = splex.points[2];
			splex.c = 1;
			D = AO;
		}
		return false;
	}

	static bool processSimplexTetrahedral(Simplex& splex, glm::vec3& D) {
		return false;
	}

	static bool(*simplexTable[])(Simplex&, glm::vec3&) = {
		nullptr,
		&processSimplexLine,
		&processSimplexTriangle,
		&processSimplexTetrahedral
	};

	static void gjk(const Mesh& A, const Mesh& B) {
		Simplex splex;
		auto S = findSupport(glm::vec3{ 1.0f }, A, B);
		splex.points[splex.c++] = S;
		auto D = -S;
		while (true) {
			S = findSupport(D, A, B);
			if (glm::dot(D, S) < 0) { return; } //does not intersect
			splex.points[splex.c++] = S;
			if (simplexTable[splex.c](splex, D)) { return; } //intersect
		}

	}


}