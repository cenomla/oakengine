#include <glm/glm.hpp>


struct CollisionInfo {
	glm::vec2 position;
	glm::vec2 extent;
};

bool aabb_vs_aabb(const CollisionInfo& a, const CollisionInfo& b, glm::vec2& normal, float& depth) {
	glm::vec2 n = b.position - a.position;
	glm::vec2 overlap = a.extent + b.extent - glm::abs(n);

	if (overlap.x > 0 && overlap.y > 0) {
		if (overlap.x < overlap.y) {
			if (n.x < 0) {
				normal = glm::vec2{ -1, 0 };
			} else {
				normal = glm::vec2{ 1, 0 };
			}
			depth = overlap.x;
		} else {
			if (n.y < 0) {
				normal = glm::vec2{ 0, -1 };
			} else {
				normal = glm::vec2{ 0, 1 };
			}
			depth = overlap.y;
		}
		return true;
	}
	return false;
}
