#include "collision.h"

#include <limits>
#include <glm/gtc/constants.hpp>
#include "oak_assert.h"

namespace oak {

	static glm::vec2 findSupport(const glm::vec2& dir, const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB) {
		float max = -std::numeric_limits<float>::max();
		glm::vec2 sA = -dir, sB = dir;
		float d;
		//find support point in mesh a
		//furthest point along vector dir
		for (const auto& v : A.vertices) {
			//transform point and test against dir
			const auto& tv = glm::vec2{ tA * glm::vec3{ v.position, 1.0f } };
			d = glm::dot(tv, dir);
			if (d > max) {
				max = d;
				sA = tv;
			}
		}
		max = -std::numeric_limits<float>::max();
		//find support point in mesh b
		for (const auto& v : B.vertices) {
			//transform point and test against -dir
			const auto& tv = glm::vec2{ tB * glm::vec3{ v.position, 1.0f } };
			d = glm::dot(tv, -dir);
			if (d > max) {
				max = d;
				sB = tv;
			}
		}
		
		return sA - sB;
	}

	glm::vec2 tripleProduct(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C) {
		return B * glm::dot(C, A) - A * glm::dot(C, B);
	}

	static bool processSimplexLine(Simplex& splex, glm::vec2& D) {
		oak_assert(splex.size() == 2);
		glm::vec2 AB = splex[0] - splex[1]; //vector from B to A
		glm::vec2 AO = -splex[1]; //vector from a to origin

		auto ABN = tripleProduct(AB, AO, AB);
		D = ABN;
		return false;
	}

	static bool processSimplexTriangle(Simplex& splex, glm::vec2& D) {
		oak_assert(splex.size() == 3);
		glm::vec2 AB = splex[1] - splex[2]; //vector from b to a
		glm::vec2 AC = splex[0] - splex[2]; //vector from c to a
		glm::vec2 AO = -splex[2]; //vector from a to origin

		auto ABN = tripleProduct(AC, AB, AB);
		auto ACN = tripleProduct(AB, AC, AC);

		if (glm::dot(ABN, AO) > 0.0f) {
			splex[0] = splex[1];
			splex[1] = splex[2];
			splex.pop_back();
			D = ABN;
		} else {
			if (glm::dot(ACN, AO) > 0.0f) {
				splex[1] = splex[2];
				splex.pop_back();
				D = ACN;
			} else {
				return true;
			}
		}
		return false;
	}

	static bool(*simplexTable[])(Simplex&, glm::vec2&) = {
		nullptr,
		&processSimplexLine,
		&processSimplexTriangle
	};

	bool gjk(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex) {
		oak_assert(splex.size() == 0);
		auto D = glm::vec2{ 1.0f };
		splex.push_back(findSupport(D, A, tA, B, tB));
		D = -D;
		while (true) {
			splex.push_back(findSupport(D, A, tA, B, tB));
			if (glm::dot(D, splex.back()) <= 0.0f) { return false; } //does not intersect
			if (simplexTable[splex.size() - 1](splex, D)) { return true; } //intersect
		}
	}

	struct SimplexEdge {
		glm::vec2 normal;
		float distance;
		int index = 0;
	};

	static void findEdge(Simplex& splex, SimplexEdge& edge) {
		//find the closest edge to the origin

		edge.distance = std::numeric_limits<float>::max();

		for (int i = 0; i < splex.size(); i++) {
			int j = i + 1 == splex.size() ? 0 : i + 1;

			auto a = splex[i];
			auto b = splex[j];

			auto e = b - a;

			auto n = glm::normalize(glm::vec2{ -e.y, e.x });

			float d = glm::dot(n, a);
			if (d < edge.distance) {
				edge.distance = d;
				edge.normal = n;
				edge.index = j;
			}
		}

	}

	void epa(const Mesh2d& A, const glm::mat3& tA, const Mesh2d& B, const glm::mat3& tB, Simplex& splex, glm::vec2& normal, float& depth) {
		while (true) {
			SimplexEdge edge;
			findEdge(splex, edge);

			auto p = findSupport(edge.normal, A, tA, B, tB);

			float dist = glm::dot(p, edge.normal);

			if (dist - edge.distance < 0.00001f) {
				normal = edge.normal;
				depth = dist;
				return;
			} else {
				//insert new points into simplex
				splex.insert(std::begin(splex) + edge.index, p);
			}

		}

	}

}