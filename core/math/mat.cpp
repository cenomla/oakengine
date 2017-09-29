#include "mat.h"

namespace oak::math {

	Mat2::Mat2(float v) : 
		value{ column_type{ v, 0.0f }, column_type{ 0.0f, v } } {}	

	Mat2::Mat2(const column_type& a, const column_type& b) :
		value{ a, b } {}

	Mat2::Mat2(const Mat3& v) :
		value{ column_type{ v.value[0] }, column_type{ v.value[1] } } {}

	Mat2::Mat2(const Mat4& v) :
		value{ column_type{ v.value[0] }, column_type{ v.value[1] } } {}

	Mat3::Mat3(float v) :
		value{ column_type{ v, 0.0f, 0.0f }, column_type{ 0.0f, v, 0.0f }, column_type{ 0.0f, 0.0f, v } } {} 

	Mat3::Mat3(const column_type& a, const column_type& b, const column_type& c) :
		value{ a, b, c } {}

	Mat3::Mat3(const Mat4& v) :
		value{ column_type{ v.value[0] }, column_type{ v.value[1] }, column_type{ v.value[2] } } {}

	Mat4::Mat4(float v) : 
		value { column_type{ v, 0.0f, 0.0f, 0.0f }, column_type{ 0.0f, v, 0.0f, 0.0f }, column_type{ 0.0f, 0.0f, v, 0.0f }, column_type{ 0.0f, 0.0f, 0.0f, v } } {}

	Mat4::Mat4(const column_type& a, const column_type& b, const column_type& c, const column_type& d) :
		value{ a, b, c, d } {}

	Mat2 operator*(const Mat2& a, const Mat2& b) {
		return Mat2{ 
			Mat2::column_type{ a.value[0] * b.value[0].x + a.value[1] * b.value[0].y },
	       		Mat2::column_type{ a.value[0] * b.value[1].x + a.value[1] * b.value[1].y } 
		};
	}

	Mat3 operator*(const Mat3& a, const Mat3& b) {
		return Mat3{
			Mat3::column_type{ a.value[0] * b.value[0].x + a.value[1] * b.value[0].y + a.value[2] * b.value[0].z },
			Mat3::column_type{ a.value[0] * b.value[1].x + a.value[1] * b.value[1].y + a.value[2] * b.value[1].z },
			Mat3::column_type{ a.value[0] * b.value[2].x + a.value[1] * b.value[2].y + a.value[2] * b.value[2].z }
		};
	}

	Mat4 operator*(const Mat4& a, const Mat4& b) {
		return Mat4{
			Mat4::column_type{ a.value[0] * b.value[0].x + a.value[1] * b.value[0].y + a.value[2] * b.value[0].z + a.value[3] * b.value[0].w },
			Mat4::column_type{ a.value[0] * b.value[1].x + a.value[1] * b.value[1].y + a.value[2] * b.value[1].z + a.value[3] * b.value[1].w },
			Mat4::column_type{ a.value[0] * b.value[2].x + a.value[1] * b.value[2].y + a.value[2] * b.value[2].z + a.value[3] * b.value[2].w },
			Mat4::column_type{ a.value[0] * b.value[3].x + a.value[1] * b.value[3].y + a.value[2] * b.value[3].z + a.value[3] * b.value[3].w }
		};
	}

	Vec2 operator*(const Mat2& a, const Vec2& b) {
		return Vec2{ a.value[0] * b.x + a.value[1] * b.y };
	}

	Vec3 operator*(const Mat3& a, const Vec3& b) {
		return Vec3{ a.value[0] * b.x + a.value[1] * b.y + a.value[2] * b.z };
	}

	Vec4 operator*(const Mat4& a, const Vec4& b) {
		return Vec4{ a.value[0] * b.x + a.value[1] * b.y + a.value[2] * b.z + a.value[3] * b.w };
	}

	float det(const Mat2& src) {
		return src.value[0].x * src.value[1].y - src.value[1].x * src.value[0].y;
	}

	float det(const Mat3& src) {
		return 
			src.value[0].x * src.value[1].y * src.value[2].z +
			src.value[1].x * src.value[2].y * src.value[0].z +
			src.value[2].x * src.value[0].y * src.value[1].z -
			src.value[2].x * src.value[1].y * src.value[0].z - 
			src.value[1].x * src.value[0].y * src.value[2].z - 
			src.value[0].x * src.value[2].y * src.value[1].z;
	}

	float det(const Mat4& src) {
		return 
			src.value[0].x * (
				src.value[1].y * src.value[2].z * src.value[3].w +
				src.value[2].y * src.value[3].z * src.value[1].w +
				src.value[3].y * src.value[1].z * src.value[2].w -
				src.value[3].y * src.value[2].z * src.value[1].w -
				src.value[2].y * src.value[1].z * src.value[3].w -
				src.value[1].y * src.value[3].z * src.value[2].w
			) - src.value[1].x * (
				src.value[0].y * src.value[2].z * src.value[3].w +
				src.value[2].y * src.value[3].z * src.value[0].w +
				src.value[3].y * src.value[0].z * src.value[2].w -
				src.value[3].y * src.value[2].z * src.value[0].w -
				src.value[2].y * src.value[0].z * src.value[3].w -
				src.value[0].y * src.value[3].z * src.value[2].w
			) + src.value[2].x * (
				src.value[0].y * src.value[1].z * src.value[3].w +
				src.value[1].y * src.value[3].z * src.value[0].w +
				src.value[3].y * src.value[0].z * src.value[1].w -
				src.value[3].y * src.value[1].z * src.value[0].w -
				src.value[1].y * src.value[0].z * src.value[3].w -
				src.value[0].y * src.value[3].z * src.value[1].w
			) - src.value[3].x * (
				src.value[0].y * src.value[1].z * src.value[2].w +
				src.value[1].y * src.value[2].z * src.value[0].w +
				src.value[2].y * src.value[0].z * src.value[1].w -
				src.value[2].y * src.value[1].z * src.value[0].w -
				src.value[1].y * src.value[0].z * src.value[2].w -
				src.value[0].y * src.value[2].z * src.value[1].w
			);
	}

	Mat2 transpose(const Mat2& src) {
		return Mat2{ 
			Mat2::column_type{ src.value[0].x, src.value[1].x }, 
			Mat2::column_type{ src.value[0].y, src.value[1].y } 
		};
	}

	Mat3 transpose(const Mat3& src) {
		return Mat3 {
			Mat3::column_type{ src.value[0].x, src.value[1].x, src.value[2].x },
			Mat3::column_type{ src.value[0].y, src.value[1].y, src.value[2].y },
			Mat3::column_type{ src.value[0].z, src.value[1].z, src.value[2].z }
		};
	}

	Mat4 transpose(const Mat4& src) {
		return Mat4 {
			Mat4::column_type{ src.value[0].x, src.value[1].x, src.value[2].x, src.value[3].x },
			Mat4::column_type{ src.value[0].y, src.value[1].y, src.value[2].y, src.value[3].y },
			Mat4::column_type{ src.value[0].z, src.value[1].z, src.value[2].z, src.value[3].z },
			Mat4::column_type{ src.value[0].w, src.value[1].w, src.value[2].w, src.value[3].w }
		};
	}

	Mat2 inverse(const Mat2& src) { }

	Mat3 inverse(const Mat3& src) { }

	Mat4 inverse(const Mat4& src) {}
}
