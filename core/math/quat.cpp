#include "quat.h"

#include <cmath>

#include "vec.h"
#include "mat.h"

namespace oak::math {

	Quat::Quat(float v) : x{ v }, y{ v }, z{ v }, w{ v } {}

	Quat::Quat(float a, float b, float c, float d) : x{ a }, y{ b }, z{ c }, w{ d } {}

	Quat::Quat(const Vec3& v) {
		Vec3 c{ cosf(v.x * 0.5f), cosf(v.y * 0.5f), cosf(v.z * 0.5f) };
		Vec3 s{ sinf(v.x * 0.5f), sinf(v.y * 0.5f), sinf(v.z * 0.5f) };	

		x = s.x * c.y * c.z - c.x * s.y * s.z;
		y = c.x * s.y * c.z + s.x * c.y * s.z;
		z = c.x * c.y * s.z - s.x * s.y * c.z;
		w = c.x * c.y * c.z + s.x * s.y * s.z;
	}

	Vec3 operator*(const Quat& a, const Vec3& b) {
		Vec3 qv{ a.x, a.y, a.z };
		Vec3 uv{ cross(qv, b) };
		Vec3 uuv{ cross(qv, uv) };

		return b + ((uv * a.w) + uuv) * 2.0f;
	}

	Mat3 toMat3(const Quat& q) {
		float qxx = q.x * q.x;
		float qyy = q.y * q.y;
		float qzz = q.z * q.z;
		float qxz = q.x * q.z;
		float qxy = q.x * q.y;
		float qyz = q.y * q.z;
		float qwx = q.w * q.x;
		float qwy = q.w * q.y;
		float qwz = q.w * q.z;

		return Mat3{
			Mat3::column_type{ 1.0f - 2.0f * (qyy + qzz), 2.0f * (qxy + qwz), 2.0f * (qxz - qwy) },
			Mat3::column_type{ 2.0f * (qxy - qwz), 1.0f - 2.0f * (qxx + qzz), 2.0f * (qyz + qwx) },
			Mat3::column_type{ 2.0f * (qxz + qwy), 2.0f * (qyz - qwx), 1.0f - 2.0f * (qxx + qyy) }
		};
	}

	Mat4 toMat4(const Quat& q) {
		return Mat4{ toMat3(q) };	
	}

};
