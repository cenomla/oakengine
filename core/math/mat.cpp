#include "mat.h"

#include <cmath>

#include "vec.h"

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

	Mat3::Mat3(const Mat2& v) :
		value{ column_type{ v.value[0], 0.0f }, column_type{ v.value[1], 0.0f }, column_type{ 0.0f, 0.0f, 1.0f } } {}
	Mat3::Mat3(const Mat4& v) :
		value{ column_type{ v.value[0] }, column_type{ v.value[1] }, column_type{ v.value[2] } } {}

	Mat4::Mat4(float v) : 
		value { column_type{ v, 0.0f, 0.0f, 0.0f }, column_type{ 0.0f, v, 0.0f, 0.0f }, column_type{ 0.0f, 0.0f, v, 0.0f }, column_type{ 0.0f, 0.0f, 0.0f, v } } {}

	Mat4::Mat4(const column_type& a, const column_type& b, const column_type& c, const column_type& d) :
		value{ a, b, c, d } {}

	Mat4::Mat4(const Mat2& v) :
		Mat4{ Mat3{ v } } {}

	Mat4::Mat4(const Mat3& v) :
		value{ column_type{ v.value[0], 0.0f }, column_type{ v.value[1], 0.0f }, column_type{ v.value[2], 0.0f }, column_type{ 0.0f, 0.0f, 0.0f, 1.0f } } {}

	Mat2 operator*(const Mat2& a, float v) {
		return Mat2{ a.value[0] * v, a.value[1] * v };
	}

	Mat3 operator*(const Mat3& a, float v) {
		return Mat3{ a.value[0] * v, a.value[1] * v, a.value[2] * v };
	}

	Mat4 operator*(const Mat4& a, float v) {
		return Mat4{ a.value[0] * v, a.value[1] * v, a.value[2] * v, a.value[3] * v };
	}

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

	Mat2 inverse(const Mat2& src) { 
		float ood = 1.0f / det(src);		

		return Mat2{
			Mat2::column_type{ src.value[1].y * ood, -src.value[0].y * ood },
			Mat2::column_type{ -src.value[1].x * ood, src.value[0].x * ood }
		};
	}

	Mat3 inverse(const Mat3& src) {
		float ood = 1.0f / det(src);

		return Mat3{
			Mat3::column_type{ 
			(src.value[1].y * src.value[2].z - src.value[2].y * src.value[1].z) * ood,
			- (src.value[1].x * src.value[2].z - src.value[2].x * src.value[1].z) * ood,
			(src.value[1].x * src.value[2].y - src.value[2].x * src.value[1].y) * ood },
			Mat3::column_type{
			- (src.value[0].y * src.value[2].z - src.value[2].y * src.value[0].z) * ood,
			(src.value[0].x * src.value[2].z - src.value[2].x * src.value[0].z) * ood,
			- (src.value[0].x * src.value[2].y - src.value[2].x * src.value[0].y) *ood },
			Mat3::column_type{
			(src.value[0].x * src.value[1].z - src.value[1].y * src.value[0].z) * ood,
			- (src.value[0].x * src.value[1].z - src.value[1].x * src.value[0].z) * ood,
			(src.value[0].x * src.value[1].y - src.value[1].x * src.value[0].y) * ood }
		};
	}

	Mat4 inverse(const Mat4& src) {
		float ood = 1.0f / det(src);

		float c00 = src.value[2].z * src.value[3].w - src.value[3].z * src.value[2].w;
		float c02 = src.value[1].z * src.value[3].w - src.value[3].z * src.value[1].w;
		float c03 = src.value[1].z * src.value[2].w - src.value[2].z * src.value[1].w;

		float c04 = src.value[2].y * src.value[3].w - src.value[3].y * src.value[2].w;	
		float c06 = src.value[1].y * src.value[3].w - src.value[3].y * src.value[1].w;
		float c07 = src.value[1].y * src.value[2].w - src.value[2].y * src.value[1].w;
		
		float c08 = src.value[2].y * src.value[3].z - src.value[3].y * src.value[2].z;
		float c10 = src.value[1].y * src.value[3].z - src.value[3].y * src.value[1].z;
		float c11 = src.value[1].y * src.value[2].z - src.value[2].y * src.value[1].z;

		float c12 = src.value[2].x * src.value[3].w - src.value[3].x * src.value[2].w;
		float c14 = src.value[1].x * src.value[3].w - src.value[3].x * src.value[1].w;
		float c15 = src.value[1].x * src.value[2].w - src.value[2].x * src.value[1].w;

		float c16 = src.value[2].x * src.value[3].z - src.value[3].x * src.value[2].z;
		float c18 = src.value[1].x * src.value[3].z - src.value[3].x * src.value[1].z;
		float c19 = src.value[1].x * src.value[2].z - src.value[2].x * src.value[1].z;

		float c20 = src.value[2].x * src.value[3].y - src.value[3].x * src.value[2].y;
		float c22 = src.value[1].x * src.value[3].y - src.value[3].x * src.value[1].y;
		float c23 = src.value[1].x * src.value[2].y - src.value[2].x * src.value[1].y;

		Mat4::column_type f0{ c00, c00, c02, c03 };
		Mat4::column_type f1{ c04, c04, c06, c07 };
		Mat4::column_type f2{ c08, c08, c10, c11 };
		Mat4::column_type f3{ c12, c12, c14, c15 };
		Mat4::column_type f4{ c16, c16, c18, c19 };
		Mat4::column_type f5{ c20, c20, c22, c23 };

		Mat4::column_type v0{ src.value[1].x, src.value[0].x, src.value[0].x, src.value[0].x };
		Mat4::column_type v1{ src.value[1].y, src.value[0].y, src.value[0].y, src.value[0].y };
		Mat4::column_type v2{ src.value[1].z, src.value[0].z, src.value[0].z, src.value[0].z };
		Mat4::column_type v3{ src.value[1].w, src.value[0].w, src.value[0].w, src.value[0].w };

		Mat4::column_type i0{ v1 * f0 - v2 * f1 + v3 * f2 };
		Mat4::column_type i1{ v0 * f0 - v2 * f3 + v3 * f4 };
		Mat4::column_type i2{ v0 * f1 - v1 * f3 + v3 * f5 };
		Mat4::column_type i3{ v0 * f2 - v1 * f4 + v2 * f5 };

		Mat4::column_type sA{ 1, -1, 1, -1 };
		Mat4::column_type sB{ -1, 1, -1, 1 };

		return Mat4{ i0 * sA, i1 * sB, i2 * sA, i3 * sB } * ood;
	}

	Mat4 perspective(float fov, float ratio, float near, float far) {
		float fsn = far - near;
		float tfovh = tan(fov / 2.0f);
		return Mat4 {
			Mat4::column_type{ 1.0f / (ratio * tfovh), 0.0f, 0.0f, 0.0f },
			Mat4::column_type{ 0.0f, 1.0f / tfovh, 0.0f, 0.0f },
			Mat4::column_type{ 0.0f, 0.0f, far / (near - far), -1.0f },
			Mat4::column_type{ 0.0f, 0.0f, -(far * near) / fsn, 0.0f }
		};	
	}

	Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
		Vec3 f{ normalize(center - eye) };
		Vec3 s{ normalize(cross(f, up)) };
		Vec3 u{ cross(s, f) };

		return Mat4 { 
			Mat4::column_type{ s.x, u.x, -f.x, 0.0f },
			Mat4::column_type{ s.y, u.y, -f.y, 0.0f },
			Mat4::column_type{ s.z, u.z, -f.z, 0.0f },
			Mat4::column_type{ -dot(s, eye), -dot(u, eye), -dot(f, eye), 1.0f }
		};
	}

	Mat4 ortho(float l, float r, float b, float t, float near, float far) {
		float rsl = r - l;
		float tsb = t - b;
		float fsn = far - near;

		return Mat4 {
			Mat4::column_type{ 2.0f / rsl, 0.0f, 0.0f, 0.0f },
			Mat4::column_type{ 0.0f, 2.0f / tsb, 0.0f, 0.0f },
			Mat4::column_type{ 0.0f, 0.0f, -2.0f / (fsn), 0.0f },
			Mat4::column_type{ -(r + l)/(rsl), -(t + b)/tsb, -(far + near)/fsn, 1.0f }
		};
	}

	Mat3 rotate(const Mat3& src, float a) {
		
	}

	Mat4 rotate(const Mat4& src, const Vec3& v) {

	}

	Mat3 translate(const Mat3& src, const Vec2& v) {
		Mat3 mTrans{ 1.0f };
		mTrans.value[2] = Vec3{ v, 1.0f };

		return mTrans * src;
	}

	Mat4 translate(const Mat4& src, const Vec3& v) {
		Mat4 mTrans{ 1.0f };
		mTrans.value[3] = Vec4{ v, 1.0f };

		return mTrans * src;
	}

	Mat3 scale(const Mat3& src, const Vec2& v) {
		Mat3 mScale{ 1.0f };
		mScale.value[0].x = v.x;
		mScale.value[1].y = v.y;

		return mScale * src;
	}

	Mat4 scale(const Mat4& src, const Vec3& v) {
		Mat4 mScale{ 1.0f };
		mScale.value[0].x = v.x;
		mScale.value[1].y = v.y;
		mScale.value[2].z = v.z;

		return mScale * src;
	}	
}
