#include "camera.h"

namespace oak::graphics {

	const TypeInfo Camera::typeInfo = makeResourceInfo<Camera>("camera");

	void pup(Puper& puper, Camera& camera, const ObjInfo& info) {}

	void Camera::view3d(const Vec3& center, const Quat& rotation) {
		view = math::toMat4( rotation );
		view.value[3] = Vec4{ center, 1.0f };
		view = math::inverse(view);
	}
	
	void Camera::view2d(const Vec2& pos, float zoom) {
		view = translate(Mat4{ 1.0f }, Vec3{ pos, 0.0f }) * math::scale(Mat4{ 1.0f }, Vec3{ zoom });
	}

}
