#pragma once

#include "vec.h"

namespace oak::math {

	struct Mat2;
	struct Mat3;
	struct Mat4;

	struct Mat2 {
		typedef Vec2 column_type;

		Mat2() = default;
		Mat2(float v);
		Mat2(
			const column_type& a, 
			const column_type& b
		);
		Mat2(const Mat3& v);
		Mat2(const Mat4& v);

		column_type value[2];
	};

	struct Mat3 {
		typedef Vec3 column_type;

		Mat3() = default;
		Mat3(float v);
		Mat3(
			const column_type& a, 
			const column_type& b, 
			const column_type& c
		);
		Mat3(const Mat4& v);

		column_type value[3];
	};

	struct Mat4 {
		typedef Vec4 column_type;

		Mat4() = default;
		Mat4(float v);
		Mat4(
			const column_type& a, 
			const column_type& b, 
			const column_type& c, 
			const column_type& d
		);

		column_type value[4];
	};

	Mat2 operator*(const Mat2& a, const Mat2& b);
	Mat3 operator*(const Mat3& a, const Mat3& b);
	Mat4 operator*(const Mat4& a, const Mat4& b);

	Vec2 operator*(const Mat2& a, const Vec2& b);
	Vec3 operator*(const Mat3& a, const Vec3& b);
	Vec4 operator*(const Mat4& a, const Vec4& b);

	float det(const Mat2& src);
	float det(const Mat3& src);
	float det(const Mat4& src);

	Mat2 transpose(const Mat2& src);
	Mat3 transpose(const Mat3& src);
	Mat4 transpose(const Mat4& src);

	Mat2 inverse(const Mat2& src);
	Mat3 inverse(const Mat3& src);
	Mat4 inverse(const Mat4& src);

	Mat4 perspective(float fov, float ratio, float near, float far);
	Mat4 lookAt(const Vec3& center, const Vec3& eye, const Vec3& up);
	Mat4 ortho(float l, float r, float b, float t, float near, float far);

	Mat4 rotate(const Mat4& src, const Vec3& v);
	Mat4 translate(const Mat4& src, const Vec3& v);
	Mat4 scale(const Mat4& src, const Vec3& v);


}
