#pragma once

#include <math.h>

#include "Math/Plane.h"

namespace EE
{
	FORCEINLINE Plane::Plane() : x(), y(), z(), d() { }

	FORCEINLINE Plane::Plane(const float & x, const float & y, const float & z, const float & d) : x(x), y(y), z(z), d(d) { }

	FORCEINLINE Plane::Plane(const Vector3 & normalizedNormal, float inD) {
		x = normalizedNormal.x;
		y = normalizedNormal.y;
		z = normalizedNormal.z;
		d = inD;
	}

	FORCEINLINE Plane Plane::FromPointNormal(const Point3 & point, const Vector3 & normal) {
		Vector3 normalizedNormal = normal.Normalized();
		return Plane(
			normalizedNormal.x,
			normalizedNormal.y,
			normalizedNormal.z,
			-Vector3::Dot(point, normalizedNormal)
		);
	}

	FORCEINLINE Plane Plane::From3Points(const Point3 & v0, const Point3 & v1, const Point3 & v2) {
		Vector3 Normal = Vector3::Cross(v1 - v0, v2 - v0);
		Normal.Normalize();
		FromPointNormal(v0, Normal);
	}

	FORCEINLINE void Plane::Normalize() {
		float distance = sqrtf(x*x + y*y + z*z);
		x /= distance;
		y /= distance;
		z /= distance;
		d /= distance;
	}

	FORCEINLINE float Plane::SignedDistance(const Vector3 &p) const {
		return (x * p.x + y * p.y + z * p.z + d);
	}

	FORCEINLINE float Plane::Dot(const Plane &p, const Vector4 &v) {
		return p.x * v.x + p.y * v.y + p.z * v.z + p.d * v.w;
	}

	FORCEINLINE float Plane::Dot(const Plane &p, const Vector3 &v) {
		return p.x * v.x + p.y * v.y + p.z * v.z + p.d;
	}

	FORCEINLINE float Plane::DotCoord(const Plane &p, const Vector3 &v) {
		return p.x * v.x + p.y * v.y + p.z * v.z + p.d;
	}

	FORCEINLINE float Plane::DotNormal(const Plane &p, const Vector3 &v) {
		return p.x * v.x + p.y * v.y + p.z * v.z;
	}
}