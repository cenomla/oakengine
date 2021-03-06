#pragma once

namespace oak::math {

	struct Vec2;
	struct Vec3;
	struct Vec4;

	struct Mat2;
	struct Mat3;
	struct Mat4;

	struct Ivec2 {
		int x = 0, y = 0;
	};

	struct Ivec3 {
		int x = 0, y = 0, z = 0;
	};

	struct Ivec4 {
		int x = 0, y = 0, z = 0, w = 0;
	};

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

	bool operator==(const Ivec2& a, const Ivec2& b);
	bool operator==(const Ivec3& a, const Ivec3& b);
	bool operator==(const Ivec4& a, const Ivec4& b);
	bool operator==(const Vec2& a, const Vec2& b);
	bool operator==(const Vec3& a, const Vec3& b);
	bool operator==(const Vec4& a, const Vec4& b);

	bool operator!=(const Ivec2& a, const Ivec2& b);
	bool operator!=(const Ivec3& a, const Ivec3& b);
	bool operator!=(const Ivec4& a, const Ivec4& b);
	bool operator!=(const Vec2& a, const Vec2& b);
	bool operator!=(const Vec3& a, const Vec3& b);
	bool operator!=(const Vec4& a, const Vec4& b);

	Ivec2 operator-(const Ivec2& v);
	Ivec3 operator-(const Ivec3& v);
	Ivec4 operator-(const Ivec4& v);
	Vec2 operator-(const Vec2& v);
	Vec3 operator-(const Vec3& v);
	Vec4 operator-(const Vec4& v);

	Ivec2 operator+(const Ivec2& a, const Ivec2& b);
	Ivec3 operator+(const Ivec3& a, const Ivec3& b);
	Ivec4 operator+(const Ivec4& a, const Ivec4& b);
	Vec2 operator+(const Vec2& a, const Vec2& b);
	Vec3 operator+(const Vec3& a, const Vec3& b);
	Vec4 operator+(const Vec4& a, const Vec4& b);

	Ivec2 operator-(const Ivec2& a, const Ivec2& b);
	Ivec3 operator-(const Ivec3& a, const Ivec3& b);
	Ivec4 operator-(const Ivec4& a, const Ivec4& b);
	Vec2 operator-(const Vec2& a, const Vec2& b);
	Vec3 operator-(const Vec3& a, const Vec3& b);
	Vec4 operator-(const Vec4& a, const Vec4& b);

	Ivec2 operator*(const Ivec2& a, int b);
	Ivec3 operator*(const Ivec3& a, int b);
	Ivec4 operator*(const Ivec4& a, int b);
	Vec2 operator*(const Vec2& a, float b);
	Vec3 operator*(const Vec3& a, float b);
	Vec4 operator*(const Vec4& a, float b);

	Ivec2 operator*(const Ivec2& a, const Ivec2& b);
	Ivec3 operator*(const Ivec3& a, const Ivec3& b);
	Ivec4 operator*(const Ivec4& a, const Ivec4& b);
	Vec2 operator*(const Vec2& a, const Vec2& b);
	Vec3 operator*(const Vec3& a, const Vec3& b);
	Vec4 operator*(const Vec4& a, const Vec4& b);
	
	Ivec2 operator/(const Ivec2& a, int b);
	Ivec3 operator/(const Ivec3& a, int b);
	Ivec4 operator/(const Ivec4& a, int b);
	Vec2 operator/(const Vec2& a, float b);
	Vec3 operator/(const Vec3& a, float b);
	Vec4 operator/(const Vec4& a, float b);

	Ivec2 operator/(const Ivec2& a, const Ivec2& b);
	Ivec3 operator/(const Ivec3& a, const Ivec3& b);
	Ivec4 operator/(const Ivec4& a, const Ivec4& b);
	Vec2 operator/(const Vec2& a, const Vec2& b);
	Vec3 operator/(const Vec3& a, const Vec3& b);
	Vec4 operator/(const Vec4& a, const Vec4& b);

	Ivec2& operator+=(Ivec2& a, const Ivec2& b);
	Ivec3& operator+=(Ivec3& a, const Ivec3& b);
	Ivec4& operator+=(Ivec4& a, const Ivec4& b);
	Vec2& operator+=(Vec2& a, const Vec2& b);
	Vec3& operator+=(Vec3& a, const Vec3& b);
	Vec4& operator+=(Vec4& a, const Vec4& b);

	Ivec2& operator-=(Ivec2& a, const Ivec2& b);
	Ivec3& operator-=(Ivec3& a, const Ivec3& b);
	Ivec4& operator-=(Ivec4& a, const Ivec4& b);
	Vec2& operator-=(Vec2& a, const Vec2& b);
	Vec3& operator-=(Vec3& a, const Vec3& b);
	Vec4& operator-=(Vec4& a, const Vec4& b);

	Ivec2& operator*=(Ivec2& a, int b);
	Ivec3& operator*=(Ivec3& a, int b);
	Ivec4& operator*=(Ivec4& a, int b);
	Vec2& operator*=(Vec2& a, float b);
	Vec3& operator*=(Vec3& a, float b);
	Vec4& operator*=(Vec4& a, float b);

	Ivec2& operator*=(Ivec2& a, const Ivec2& b);
	Ivec3& operator*=(Ivec3& a, const Ivec3& b);
	Ivec4& operator*=(Ivec4& a, const Ivec4& b);
	Vec2& operator*=(Vec2& a, const Vec2& b);
	Vec3& operator*=(Vec3& a, const Vec3& b);
	Vec4& operator*=(Vec4& a, const Vec4& b);
	
	Ivec2& operator/=(Ivec2& a, int b);
	Ivec3& operator/=(Ivec3& a, int b);
	Ivec4& operator/=(Ivec4& a, int b);
	Vec2& operator/=(Vec2& a, float b);
	Vec3& operator/=(Vec3& a, float b);
	Vec4& operator/=(Vec4& a, float b);

	Ivec2& operator/=(Ivec2& a, const Ivec2& b);
	Ivec3& operator/=(Ivec3& a, const Ivec3& b);
	Ivec4& operator/=(Ivec4& a, const Ivec4& b);
	Vec2& operator/=(Vec2& a, const Vec2& b);
	Vec3& operator/=(Vec3& a, const Vec3& b);
	Vec4& operator/=(Vec4& a, const Vec4& b);

	float dot(const Vec2& a, const Vec2& b);
	float dot(const Vec3& a, const Vec3& b);
	float dot(const Vec4& a, const Vec4& b);

	Mat2 outerProduct(const Vec2& a, const Vec2& b);
	Mat3 outerProduct(const Vec3& a, const Vec3& b);
	Mat4 outerProduct(const Vec4& a, const Vec4& b);

	Vec3 cross(const Vec3& a, const Vec3& b);

	Vec2 vectorTriple(const Vec2& a, const Vec2& b, const Vec2& c);
	Vec3 vectorTriple(const Vec3& a, const Vec3& b, const Vec3& c);

	float scalarTriple(const Vec3& a, const Vec3& b, const Vec3& c);

	float length(const Vec2& v);
	float length(const Vec3& v);
	float length(const Vec4& v);

	Vec2 normalize(const Vec2& v);
	Vec3 normalize(const Vec3& v);
	Vec4 normalize(const Vec4& v);

}
