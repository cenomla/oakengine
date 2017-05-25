#include "mesh.h"

#include "util/file_util.h"
#include "util/string_util.h"

namespace oak::graphics {

	Mesh::Mesh(const MeshDescriptor& descriptor) : descriptor_{ descriptor } {}

	void Mesh::load(const oak::string &path) {

		oak::string file = util::readFileAsString(path);
		oak::vector<oak::string> tokens;

		oak::vector<glm::vec3> vertices;
		oak::vector<glm::vec3> normals;
		oak::vector<glm::vec2> uvs;
		oak::vector<glm::ivec3> faces;

		//get vertices and face indices from obj file
		size_t prev = 0, pos;
		do {
			pos = file.find('\n', prev);
			const auto& line = file.substr(prev, pos - prev);
			prev = pos + 1;
			if (line[0] == 'v') {
				if (line[1] == ' ') {
					//parse vertices
					util::splitstr(line, " ", tokens);
					//put vertices data into vertices vector
					vertices.push_back(glm::vec3{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()), std::stof(tokens[3].c_str()) });
					tokens.clear();
				}
				else if (line[1] == 't') {
					//parse texture coords
					util::splitstr(line, " ", tokens);
					//put texcoord data into uvs vector
					uvs.push_back(glm::vec2{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()) });
					tokens.clear();
				}
				else if (line[1] == 'n') {
					//parse normals
					util::splitstr(line, " ", tokens);
					//put normal data into normal vector
					normals.push_back(glm::vec3{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()), std::stof(tokens[3].c_str()) });
					tokens.clear();
				}
			}
			else if (line[0] == 'f') {
				util::splitstr(line, " ", tokens);
				oak::vector<oak::string> indices;
				//parse first point in the face
				for (int i = 1; i <= 3; i++) {
					util::splitstr(tokens[i], "/", indices);
					faces.push_back(glm::ivec3{ std::stoi(indices[0].c_str()), std::stoi(indices[1].c_str()), std::stoi(indices[2].c_str()) });
					indices.clear();
				}
				tokens.clear();
			}
		} while (pos != oak::string::npos);

		oak::vector<glm::vec3> normalList{ vertices.size() };
		oak::vector<glm::vec2> uvList{ vertices.size() };
		oak::vector<uint32_t> indexList;
		//process faces
		for (const auto& face : faces) {
			uint32_t vi = face.x - 1;
			indexList.push_back(vi);
			normalList[vi] = normals[face.z - 1];
			uvList[vi] = glm::vec2{ uvs[face.y - 1].x, 1.0f - uvs[face.y - 1].y };
		}

		size_t i = 0;
		for (const auto& vertex : vertices) {
			vertices_.push_back(MeshVertex{ vertex, normalList[i], uvList[i] });
			i++;
		}
		for (const auto& index : indexList) {
			indices_.push_back(index);
		}

	}

	void Mesh::draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const {
		MeshVertex *data = static_cast<MeshVertex*>(buffer);
		uint32_t *idata = static_cast<uint32_t*>(ibuffer);

		size_t i = 0;
		for (const auto& vertex : vertices_) {
			data[i++] = { glm::vec3{ transform * glm::vec4{ vertex.position, 1.0f } }, glm::vec3{ transform * glm::vec4{ vertex.normal, 0.0f } }, vertex.uvs };
		}
		i = 0;
		for (const auto& index : indices_) {
			idata[i++] = startIndex + index;
		}
	}

}