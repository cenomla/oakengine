#include "vec.h"

#include <cmath>

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

	Vec2 operator+(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x + b.x, a.y + b.y };
	}

	Vec3 operator+(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
	}

	Vec4 operator+(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
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

	Vec2 operator-(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x - b.x, a.y - b.y };
	}

	Vec3 operator-(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
	}

	Vec4 operator-(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
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

	Vec2 operator*(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x * b.x, a.y * b.y };
	}

	Vec3 operator*(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x * b.x, a.y * b.y, a.z * b.z };
	}

	Vec4 operator*(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
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

	Vec2 operator/(const Vec2& a, const Vec2& b) {
		return Vec2{ a.x / b.x, a.y / b.y };
	}

	Vec3 operator/(const Vec3& a, const Vec3& b) {
		return Vec3{ a.x / b.x, a.y / b.y, a.z / b.z };
	}

	Vec4 operator/(const Vec4& a, const Vec4& b) {
		return Vec4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	}

	float dot(const Vec2& a, const Vec2& b) {
		return (a.x * b.x) + (a.y + b.y);
	}

	float dot(const Vec3& a, const Vec3& b) {
		return (a.x * b.x) + (a.y + b.y) + (a.z + b.z);
	}

	Vec3 cross(const Vec3& a, const Vec3& b) {
		return Vec3{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
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
