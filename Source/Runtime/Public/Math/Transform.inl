#include "CoreMinimal.h"
#include "Math/Transform.h"

namespace EE::Math
{
    template <typename T>
	TTransform<T>::TTransform() : position(), rotation(), scale( 1.F )
	{
	}

    template <typename T>
	TTransform<T>::TTransform( const TPoint3<T>& position, const TQuaternion<T>& rotation, const TVector3<T>& scale ) :
		position( position ), rotation( rotation ), scale( scale )
	{
	}

    template <typename T>
	TTransform<T>::TTransform( const TPoint3<T>& position, const TQuaternion<T>& rotation ) :
		position( position ), rotation( rotation ), scale( 1.F )
	{
	}

    template <typename T>
	TTransform<T>::TTransform( const TPoint3<T>& position ) :
		position( position ), rotation(), scale( 1 )
	{
	}

    template <typename T>
	TTransform<T>::TTransform( const TMatrix4x4<T>& matrix )
	{
		position = matrix.ExtractTranslation();
		rotation = matrix.ExtractRotation();
		scale = matrix.ExtractScale();
	}

    template <typename T>
	TMatrix4x4<T> TTransform<T>::GetWorldToLocalMatrix() const
	{
		return GetLocalToWorldMatrix().Inversed();
	}

    template <typename T>
	TMatrix4x4<T> TTransform<T>::GetLocalToWorldMatrix() const
	{
		return TMatrix4x4::Translation( position ) * rotation.ToMatrix4x4() * TMatrix4x4::Scaling( scale );
	}

    template <typename T>
	TMatrix4x4<T> TTransform<T>::GetViewMatrix() const
	{
		TVector3<T> const forward( rotation * TVector3<T>( T(0), T(0), T(1) ) );
		TVector3<T> const side( forward.Cross( rotation * TVector3<T>( T(0), T(1) ) ) );
		TVector3<T> const upper( side.Cross( forward ) );
		
		return TMatrix4x4<T>
		(
		    side.x, upper.x, forward.x, T(0),
		    side.y, upper.y, forward.y, T(0),
		    side.z, upper.z, forward.z, T(0),
		    -side.Dot( position ), -upper.Dot( position ), -forward.Dot( position ), T(1)
		);
	}

    template <typename T>
	TVector3<T> TTransform<T>::Forward() const
	{
		return rotation * TVector3<T>( T(0), T(0), T(1) );
	}

    template <typename T>
	TVector3<T> TTransform<T>::Up() const
	{
		return rotation * TVector3<T>( T(0), T(1) );
	}

    template <typename T>
	TVector3<T> TTransform<T>::Right() const
	{
		return rotation * TVector3<T>( T(1), T(0) );
	}

    template <typename T>
	TTransform<T> TTransform<T>::operator*( const TTransform<T>& other ) const
	{
		TTransform<T> result;

		TVector3<T> resultScale = other.scale * scale;

		TQuaternion<T> inverseRot = rotation.Inversed();
		TQuaternion<T> resultRotation = other.rotation * rotation;

		TVector3<T> rotatedTranslation = other.rotation * position;

		TVector3<T> resultTranslation = (resultScale * rotatedTranslation) + other.position;

		result.scale = resultScale;
		result.position = resultTranslation;
		result.rotation = resultRotation;
		return result;
	}
}