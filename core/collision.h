#pragma once

#include <glm/glm.hpp>
#include "mesh2d.h"

namespace oak {

	typedef oak::vector<glm::vec2> Simplex;

	bool gjk(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex);
	void epa(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex, glm::vec2& normal, float& depth);

}