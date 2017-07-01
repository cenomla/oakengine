#include "mesh.h"

#include "util/file_util.h"
#include "util/string_util.h"

namespace oak::graphics {

	Mesh::Mesh(const AttributeLayout *layout) : layout_{ layout }, data_{ nullptr }, idata_{ nullptr } {}

	Mesh::~Mesh() {
		if (data_) {
			oak_allocator.deallocate(data_, vertexCount_ * layout_->stride());
		}
		if (idata_) {
			oak_allocator.deallocate(idata_, indexCount_ * 4);
		}
		data_ = nullptr;
		idata_ = nullptr;
	}

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
		data_ = oak_allocator.allocate(vertices.size() * 32);
		idata_ = oak_allocator.allocate(indexList.size() * 4);

		struct MeshVertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uvs;
		};
		for (const auto& vertex : vertices) {
			static_cast<MeshVertex*>(data_)[i] = MeshVertex{ vertex, normalList[i], uvList[i] };
			i++;
		}
		vertexCount_ = i;
		i = 0;
		for (const auto& index : indexList) {
			static_cast<uint32_t*>(idata_)[i++] = index;
		}
		indexCount_ = i;
	}

	static size_t size[] = {
		12,
		8,
		12, 
		8, 
		4
	};

	void Mesh::setData(void *vertices, void *indices, size_t vcount, size_t icount) {
		if (data_) {
			oak_allocator.deallocate(data_, vertexCount_ * layout_->stride());
		}
		if (idata_) {
			oak_allocator.deallocate(idata_, indexCount_ * 4);
		}
		data_ = vertices;
		idata_ = indices;
		vertexCount_ = vcount;
		indexCount_ = icount;
	}

	void Mesh::draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const {

		oak::vector<std::pair<size_t, int>> attributes;

		size_t offset = 0;
		for (const auto& attr : layout_->attributes) {
			switch (attr) {
				case AttributeType::POSITION:
					attributes.push_back({ offset, 1 });
					break;
				case AttributeType::NORMAL:
					attributes.push_back({ offset, 0 });
					break;
				default:
					attributes.push_back({ offset, -static_cast<int>(size[static_cast<int>(attr)]) });
			}
			offset += size[static_cast<int>(attr)];
		}

		//transform stuff
		const size_t stride = layout_->stride();
		for (const auto& attr : attributes) {
			for (size_t i = 0; i < vertexCount_; i++) {
				auto b = static_cast<char*>(buffer) + i * stride;
				auto p = static_cast<char*>(data_) + i * stride;
				if (attr.second < 0) {
					size_t v = static_cast<size_t>(-attr.second);
					memcpy(b + attr.first, p + attr.first, v);
				} else {
					*reinterpret_cast<glm::vec3*>(b + attr.first) = glm::vec3{ transform * glm::vec4{ *reinterpret_cast<glm::vec3*>(p + attr.first), static_cast<float>(attr.second) } };
				}
			}
		}
		for (size_t i = 0; i < indexCount_; i++) {
			static_cast<uint32_t*>(ibuffer)[i] = static_cast<uint32_t*>(idata_)[i] + startIndex;
		}
	}

}