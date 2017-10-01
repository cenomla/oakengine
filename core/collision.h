#pragma once

#include "math.h"
#include "container.h"

namespace oak {

	struct Mesh2d {
		struct Vertex {
			Vec2 position;
			Vec2 normal;
		};

		oak::vector<Vertex> vertices;
	};

	typedef oak::vector<Vec2> Simplex;

	bool gjk(const Mesh2d& A, const Mat3& tA, const Mesh2d& B, const Mat3& tB, Simplex& splex);
	void epa(const Mesh2d& A, const Mat3& tA, const Mesh2d& B, const Mat3& tB, Simplex& splex, Vec2& normal, float& depth);

}
