#include "CoreMinimal.h"
#include "Math/Transform.h"

namespace EEngine
{
	Transform::Transform() : position(), rotation(), scale( 1.F )
	{
	}

	Transform::Transform( const Point3& position, const Quaternion& rotation, const Vector3& scale ) :
		position( position ), rotation( rotation ), scale( scale )
	{
	}

	Transform::Transform( const Point3& position, const Quaternion& rotation ) :
		position( position ), rotation( rotation ), scale( 1.F )
	{
	}

	Transform::Transform( const Point3& position ) :
		position( position ), rotation(), scale( 1.F )
	{
	}

	Transform::Transform( const Matrix4x4& matrix )
	{
		position = matrix.ExtractTranslation();
		rotation = matrix.ExtractRotation();
		scale = matrix.ExtractScale();
	}

	Matrix4x4 Transform::GetWorldToLocalMatrix() const
	{
		return GetLocalToWorldMatrix().Inversed();
	}

	Matrix4x4 Transform::GetLocalToWorldMatrix() const
	{
		return Matrix4x4::Translation( position ) * rotation.ToMatrix4x4() * Matrix4x4::Scaling( scale );
	}

	Matrix4x4 Transform::GetViewMatrix() const
	{
		Vector3 const forward( rotation * Vector3( 0, 0, 1 ) );
		Vector3 const side( forward.Cross( rotation * Vector3( 0, 1 ) ) );
		Vector3 const upper( side.Cross( forward ) );
		
		return Matrix4x4
		(
		    side.x, upper.x, -forward.x, 0,
		    side.y, upper.y, -forward.y, 0,
		    side.z, upper.z, -forward.z, 0,
		    -side.Dot( position ), -upper.Dot( position ), forward.Dot( position ), 1
		);
	}

	Vector3 Transform::Forward() const
	{
		return rotation * Vector3( 0, 0, 1 );
	}

	Vector3 Transform::Up() const
	{
		return rotation * Vector3( 0, 1 );
	}

	Vector3 Transform::Right() const
	{
		return rotation * Vector3( 1, 0 );
	}

	Transform Transform::operator*( const Transform& other ) const
	{
		Transform result;

		Vector3 resultScale = other.scale * scale;

		Quaternion inverseRot = rotation.Inversed();
		Quaternion resultRotation = other.rotation * rotation;

		Vector3 rotatedTranslation = other.rotation * position;

		Vector3 resultTranslation = (resultScale * rotatedTranslation) + other.position;

		result.scale = resultScale;
		result.position = resultTranslation;
		result.rotation = resultRotation;
		return result;
	}
}