#pragma once

#include <glm/glm.hpp>

#include "container.h"

namespace oak {

	struct Mesh2d {
		struct Vertex {
			glm::vec2 position;
			glm::vec2 normal;
		};

		oak::vector<Vertex> vertices;
	};

	typedef oak::vector<glm::vec2> Simplex;

	bool gjk(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex);
	void epa(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex, glm::vec2& normal, float& depth);

}