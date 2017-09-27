#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace oak::graphics {

	const TypeInfo Camera::typeInfo = makeResourceInfo<Camera>("camera");

	void pup(Puper& puper, Camera& camera, const ObjInfo& info) {}

	void Camera::view3d(const glm::vec3& center, const glm::quat& rotation) {
		view = glm::mat4_cast( rotation );
		view[3] = glm::vec4{ center, 1.0f };
		view = glm::inverse(view);
	}
	
	void Camera::view2d(const glm::vec2& pos, float zoom) {
		view = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ pos, 0.0f }) * glm::scale(glm::mat4{ 1.0f }, glm::vec3{ zoom });
	}

}
