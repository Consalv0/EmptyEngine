#pragma once

#include "Math/MathUtils.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

namespace EEngine
{
	class Transform
	{
	public:
		Point3 position;
		Quaternion rotation;
		Vector3 scale;

		Transform();
		Transform( const Point3& position, const Quaternion& rotation, const Vector3& scale );
		Transform( const Point3& position, const Quaternion& rotation );
		Transform( const Point3& position );
		Transform( const Matrix4x4& matrix );

		Vector3 Forward() const;
		Vector3 Up() const;
		Vector3 Right() const;

		Transform operator*( const Transform& other ) const;

		//* Get the inverse of the Model matrix
		Matrix4x4 GetWorldToLocalMatrix() const;

		//* Get the Model matrix
		Matrix4x4 GetLocalToWorldMatrix() const;

		//* Get the Model matrix
		Matrix4x4 GetViewMatrix() const;
	};
}