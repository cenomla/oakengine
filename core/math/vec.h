#pragma once

namespace oak::math {

	struct Vec3;
	struct Vec4;

	struct Vec2 {
		Vec2() = default;
		Vec2(float a, float b);
		Vec2(float v);
		Vec2(const Vec3& v);
		Vec2(const Vec4& v);

		float x = 0.0f, y = 0.0f;
	};

	struct Vec3 {
		Vec3() = default;
		Vec3(float a, float b, float c);
		Vec3(float v);
		Vec3(const Vec4& v);
		Vec3(const Vec2& v, float a);

		float x = 0.0f, y = 0.0f, z = 0.0f;
	};

	struct Vec4 {
		Vec4() = default;
		Vec4(float a, float b, float c, float d);
		Vec4(float v);
		Vec4(const Vec2& v, float a, float b);
		Vec4(const Vec3& v, float a);
		Vec4(const Vec2& a, const Vec2& b);

		float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
	};

	Vec2 operator+(const Vec2& a, const Vec2& b);
	Vec3 operator+(const Vec3& a, const Vec3& b);
	Vec4 operator+(const Vec4& a, const Vec4& b);

	Vec2 operator-(const Vec2& v);
	Vec3 operator-(const Vec3& v);
	Vec4 operator-(const Vec4& v);

	Vec2 operator-(const Vec2& a, const Vec2& b);
	Vec3 operator-(const Vec3& a, const Vec3& b);
	Vec4 operator-(const Vec4& a, const Vec4& b);

	Vec2 operator*(const Vec2& a, float b);
	Vec3 operator*(const Vec3& a, float b);
	Vec4 operator*(const Vec4& a, float b);

	Vec2 operator*(const Vec2& a, const Vec2& b);
	Vec3 operator*(const Vec3& a, const Vec3& b);
	Vec4 operator*(const Vec4& a, const Vec4& b);
	
	Vec2 operator/(const Vec2& a, float b);
	Vec3 operator/(const Vec3& a, float b);
	Vec4 operator/(const Vec4& a, float b);

	Vec2 operator/(const Vec2& a, const Vec2& b);
	Vec3 operator/(const Vec3& a, const Vec3& b);
	Vec4 operator/(const Vec4& a, const Vec4& b);

	float dot(const Vec2& a, const Vec2& b);
	float dot(const Vec3& a, const Vec3& b);

	Vec3 cross(const Vec3& a, const Vec3& b);

	float length(const Vec2& v);
	float length(const Vec3& v);
	float length(const Vec4& v);

	Vec2 normalize(const Vec2& v);
	Vec3 normalize(const Vec3& v);
	Vec4 normalize(const Vec4& v);

}
