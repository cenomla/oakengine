#include "vec.h"

#include <cmath>

#include "mat.h"

namespace oak::math {

	Vec2::Vec2(float a, float b) : x{ a }, y{ b } {}

	Vec2::Vec2(float v) : Vec2{ v, v } {}

	Vec2::Vec2(const Vec3& v) : Vec2{ v.x, v.y } {}
	
	Vec2::Vec2(const Vec4& v) : Vec2{ v.x, v.y } {}

	Vec3::Vec3(float a, float b, float c) : x{ a }, y{ b }, z{ c } {}

	Vec3::Vec3(float v) : Vec3{ v, v, v } {}

	Vec3::Vec3(const Vec4& v) : Vec3{ v.x, v.y, v.z } {}

	Vec3::Vec3(const Vec2& v, float a) : Vec3{ v.x, v.y, a } {}

	Vec4::Vec4(float a, float b, float c, float d) : x{ a }, y{ b }, z{ c }, w{ d } {}

	Vec4::Vec4(float v) : Vec4{ v, v, v, v } {}

	Vec4::Vec4(const Vec2& v, float a, float b) : Vec4{ v.x, v.y, a, b } {}
	
	Vec4::Vec4(const Vec3& v, float a) : Vec4{ v.x, v.y, v.z, a } {}

	Vec4::Vec4(const Vec2& a, const Vec2& b) : Vec4{ a.x, a.y, b.x, b.y } {}

	bool operator==(const Ivec2& a, const Ivec2& b) {
		return a.x == b.x && a.y == b.y;
	}

	bool operator==(const Ivec3& a, const Ivec3& b) {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool operator==(const Ivec4& a, const Ivec4& b) {
		return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
	}

	bool operator==(const Vec2& a, const Vec2& b) {
		return a.x == b.x && a.y == b.y;
	}

	bool operator==(const Vec3& a, const Vec3& b) {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool operator==(const Vec4& a, const Vec4& b) {
		return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
	}

	bool operator!=(const Ivec2& a, const Ivec2& b) {
		return a.x != b.x || a.y != b.y;
	}

	bool operator!=(const Ivec3& a, const Ivec3& b) {
		return a.x != b.x || a.y != b.y || a.z != b.z;
	}

	bool operator!=(const Ivec4& a, const Ivec4& b) {
		return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
	}

	bool operator!=(const Vec2& a, const Vec2& b) {
		return a.x != b.x || a.y != b.y;
	}

	bool operator!=(const Vec3& a, const Vec3& b) {
		return a.x != b.x || a.y != b.y || a.z != b.z;
	}

	bool operator!=(const Vec4& a, const Vec4& b) {
		return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
	}

	Ivec2 operator-(const Ivec2& v) {
		return Ivec2{ -v.x, -v.y };
	}

	Ivec3 operator-(const Ivec3& v) {
		return Ivec3{ -v.x, -v.y, -v.z };
	}

	Ivec4 operator-(const Ivec4& v) {
		return Ivec4{ -v.x, -v.y, -v.z, -v.w };
	}

	Vec2 operator-(const Vec2& v) {
		return Vec2{ -v.x, -v.y };
	}

	Vec3 operator-(const Vec3& v) {
		return Vec3{ -v.x, -v.y, -v.z };
	}

	Vec4 operator-(const Vec4& v) {
		return Vec4{ -v.x, -v.y, -v.z, -v.w };
	}

	Ivec2 operator+(const Ivec2& a, const Ivec2& b) {
		return Ivec2{ a.x + b.x, a.y + b.y };
	}

	Ivec3 operator+(const Ivec3& a, const Ivec3& b) {
		return Ivec3{ a.x + b.x, a.y + b.y, a.z + b.z };
	}

	Ivec4 operator+(const Ivec4& a, const Ivec4& b) {
		return Ivec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	}

	Vec2 operator+(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x + b.x, a.y + b.y };
	}

	Vec3 operator+(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
	}

	Vec4 operator+(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	}

	Ivec2 operator-(const Ivec2& a, const Ivec2& b) {
		return Ivec2{ a.x - b.x, a.y - b.y };
	}

	Ivec3 operator-(const Ivec3& a, const Ivec3& b) {
		return Ivec3{ a.x - b.x, a.y - b.y, a.z - b.z };
	}

	Ivec4 operator-(const Ivec4& a, const Ivec4& b) {
		return Ivec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	}

	Vec2 operator-(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x - b.x, a.y - b.y };
	}

	Vec3 operator-(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
	}

	Vec4 operator-(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	}

	Ivec2 operator*(const Ivec2& a, int b) {
		return Ivec2{ a.x * b, a.y * b };
	}

	Ivec3 operator*(const Ivec3& a, int b) {
		return Ivec3{ a.x * b, a.y * b, a.z * b };
	}

	Ivec4 operator*(const Ivec4& a, int b) {
		return Ivec4{ a.x * b, a.y * b, a.z * b, a.w * b };
	}

	Vec2 operator*(const Vec2& a, float b) {
		return Vec2{ a.x * b, a.y * b };
	}

	Vec3 operator*(const Vec3& a, float b) {
		return Vec3{ a.x * b, a.y * b, a.z * b };
	}

	Vec4 operator*(const Vec4& a, float b) {
		return Vec4{ a.x * b, a.y * b, a.z * b, a.w * b };
	}

	Ivec2 operator*(const Ivec2& a, const Ivec2& b) {
		return Ivec2{ a.x * b.x, a.y * b.y };
	}

	Ivec3 operator*(const Ivec3& a, const Ivec3& b) {
		return Ivec3{ a.x * b.x, a.y * b.y, a.z * b.z };
	}

	Ivec4 operator*(const Ivec4& a, const Ivec4& b) {
		return Ivec4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	}
	
	Vec2 operator*(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x * b.x, a.y * b.y };
	}

	Vec3 operator*(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x * b.x, a.y * b.y, a.z * b.z };
	}

	Vec4 operator*(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	}
	
	Ivec2 operator/(const Ivec2& a, int b) {
		return Ivec2{ a.x / b, a.y / b };
	}

	Ivec3 operator/(const Ivec3& a, int b) {
		return Ivec3{ a.x / b, a.y / b, a.z / b };
	}

	Ivec4 operator/(const Ivec4& a, int b) {
		return Ivec4{ a.x / b, a.y / b, a.z / b, a.w / b };
	}

	Vec2 operator/(const Vec2& a, float b) {
		return Vec2{ a.x / b, a.y / b };
	}

	Vec3 operator/(const Vec3& a, float b) {
		return Vec3{ a.x / b, a.y / b, a.z / b };
	}

	Vec4 operator/(const Vec4& a, float b) {
		return Vec4{ a.x / b, a.y / b, a.z / b, a.w / b };
	}

	Ivec2 operator/(const Ivec2& a, const Ivec2& b) {
		return Ivec2{ a.x / b.x, a.y / b.y };
	}

	Ivec3 operator/(const Ivec3& a, const Ivec3& b) {
		return Ivec3{ a.x / b.x, a.y / b.y, a.z / b.z };
	}

	Ivec4 operator/(const Ivec4& a, const Ivec4& b) {
		return Ivec4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	}

	Vec2 operator/(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x / b.x, a.y / b.y };
	}

	Vec3 operator/(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x / b.x, a.y / b.y, a.z / b.z };
	}

	Vec4 operator/(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	}

	Ivec2& operator+=(Ivec2& a, const Ivec2& b) {
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	Ivec3& operator+=(Ivec3& a, const Ivec3& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	Ivec4& operator+=(Ivec4& a, const Ivec4& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	Vec2& operator+=(Vec2& a, const Vec2& b) {
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	Vec3& operator+=(Vec3& a, const Vec3& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	Vec4& operator+=(Vec4& a, const Vec4& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	Ivec2& operator-=(Ivec2& a, const Ivec2& b) {
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	Ivec3& operator-=(Ivec3& a, const Ivec3& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	Ivec4& operator-=(Ivec4& a, const Ivec4& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	Vec2& operator-=(Vec2& a, const Vec2& b) {
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	Vec3& operator-=(Vec3& a, const Vec3& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	Vec4& operator-=(Vec4& a, const Vec4& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	Ivec2& operator*=(Ivec2& a, int b) {
		a.x *= b;
		a.y *= b;
		return a;
	}

	Ivec3& operator*=(Ivec3& a, int b) {
		a.x *= b;
		a.y *= b;
		a.z *= b;
		return a;
	}

	Ivec4& operator*=(Ivec4& a, int b) {
		a.x *= b;
		a.y *= b;
		a.z *= b;
		a.w *= b;
		return a;
	}

	Vec2& operator*=(Vec2& a, float b) {
		a.x *= b;
		a.y *= b;
		return a;
	}

	Vec3& operator*=(Vec3& a, float b) {
		a.x *= b;
		a.y *= b;
		a.z *= b;
		return a;
	}

	Vec4& operator*=(Vec4& a, float b) {
		a.x *= b;
		a.y *= b;
		a.z *= b;
		a.w *= b;
		return a;
	}

	Ivec2& operator*=(Ivec2& a, const Ivec2& b) {
		a.x *= b.x;
		a.y *= b.y;
		return a;
	}

	Ivec3& operator*=(Ivec3& a, const Ivec3& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		return a;
	}

	Ivec4& operator*=(Ivec4& a, const Ivec4& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		a.w *= b.w;
		return a;
	}

	Vec2& operator*=(Vec2& a, const Vec2& b) {
		a.x *= b.x;
		a.y *= b.y;
		return a;
	}

	Vec3& operator*=(Vec3& a, const Vec3& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		return a;
	}

	Vec4& operator*=(Vec4& a, const Vec4& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		a.w *= b.w;
		return a;
	}
	
	Ivec2& operator/=(Ivec2& a, int b) {
		a.x /= b;
		a.y /= b;
		return a;
	}

	Ivec3& operator/=(Ivec3& a, int b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;
		return a;
	}

	Ivec4& operator/=(Ivec4& a, int b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;
		a.w /= b;
		return a;
	}

	Vec2& operator/=(Vec2& a, float b) {
		a.x /= b;
		a.y /= b;
		return a;
	}

	Vec3& operator/=(Vec3& a, float b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;
		return a;
	}

	Vec4& operator/=(Vec4& a, float b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;
		a.w /= b;
		return a;
	}

	Ivec2& operator/=(Ivec2& a, const Ivec2& b) {
		a.x /= b.x;
		a.y /= b.y;
		return a;
	}
	
	Ivec3& operator/=(Ivec3& a, const Ivec3& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		return a;
	}

	Ivec4& operator/=(Ivec4& a, const Ivec4& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		a.w /= b.w;
		return a;
	}

	Vec2& operator/=(Vec2& a, const Vec2& b) {
		a.x /= b.x;
		a.y /= b.y;
		return a;
	}

	Vec3& operator/=(Vec3& a, const Vec3& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		return a;
	}

	Vec4& operator/=(Vec4& a, const Vec4& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		a.w /= b.w;
		return a;
	}

	float dot(const Vec2& a, const Vec2& b) {
		return (a.x * b.x) + (a.y * b.y);
	}

	float dot(const Vec3& a, const Vec3& b) {
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	float dot(const Vec4& a, const Vec4& b) {
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	Mat2 outerProduct(const Vec2& a, const Vec2& b) {
		return Mat2 {
			a * b.x,
			a * b.y
		};
	}

	Mat3 outerProduct(const Vec3& a, const Vec3& b) {
		return Mat3 {
			a * b.x,
			a * b.y,
			a * b.z
		};
	}

	Mat4 outerProduct(const Vec4& a, const Vec4& b) {
		return Mat4 {
			a * b.x,
			a * b.y,
			a * b.z,
			a * b.w
		};
	}

	Vec3 cross(const Vec3& a, const Vec3& b) {
		return Vec3{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	Vec2 vectorTriple(const Vec2& a, const Vec2& b, const Vec2& c) {
		return b * dot(c, a) - a * dot(c, b);
	}

	Vec3 vectorTriple(const Vec3& a, const Vec3& b, const Vec3& c) {
		return cross(a, cross(b, c));
	}

	float scalarTriple(const Vec3& a, const Vec3& b, const Vec3& c) {
		return dot(cross(a, b), c);
	}	

	float length(const Vec2& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float length(const Vec3& v) {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	float length(const Vec4& v) {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	Vec2 normalize(const Vec2& v) {
		return v / length(v);
	}

	Vec3 normalize(const Vec3& v) {
		return v / length(v);
	}

	Vec4 normalize(const Vec4& v) {
		return v / length(v);
	}

}
