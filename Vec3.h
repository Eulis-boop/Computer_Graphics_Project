#pragma once
#include <cmath>
#include <array>

class Vec3
{
public: 
	float x, y, z;

	Vec3 operator-(const Vec3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}
	Vec3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}
	Vec3 cross(const Vec3& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}
	float dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}
	float dot_negative(const Vec3& other) const {
		return -x * other.x + -y * other.y + -z * other.z;
	}
	Vec3 normalize() const {
		float len = std::sqrtf(x * x + y * y + z * z);
		return { x / len, y / len, z / len };
	}
};

