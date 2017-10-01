#pragma once

#include "math.h"
#include "resource.h"

namespace oak::graphics {

	struct Camera {
		static const TypeInfo typeInfo;

		Mat4 proj{ 1.0f };
		Mat4 view{ 1.0f };

		void view3d(const Vec3& center, const Quat& rotation);
		void view2d(const Vec2& pos, float zoom);
	};

	void pup(Puper& puper, Camera& camera, const ObjInfo& info);

}
