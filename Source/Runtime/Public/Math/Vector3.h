#pragma once

#include "CoreTypes.h"

namespace EE
{

	struct IntVector3;
	struct Vector2;
	struct Vector4;

	struct Vector3
	{
	public:
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
		};

		HOST_DEVICE FORCEINLINE Vector3();
		HOST_DEVICE FORCEINLINE Vector3( const Vector2& vector );
		HOST_DEVICE FORCEINLINE Vector3( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE Vector3( const Vector3& vector );
		HOST_DEVICE FORCEINLINE Vector3( const Vector4& vector );
		HOST_DEVICE FORCEINLINE Vector3( const float& value );
		HOST_DEVICE FORCEINLINE Vector3( const float& x, const float& y, const float& z );
		HOST_DEVICE FORCEINLINE Vector3( const float& x, const float& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline float MagnitudeSquared() const;
		HOST_DEVICE inline void Normalize();
		HOST_DEVICE inline Vector3 Normalized() const;

		HOST_DEVICE FORCEINLINE Vector3 Cross( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE static Vector3 Cross( const Vector3& a, const Vector3& b );
		HOST_DEVICE FORCEINLINE float Dot( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE static float Dot( const Vector3& a, const Vector3& b );
		HOST_DEVICE FORCEINLINE static Vector3 Lerp( const Vector3& start, const Vector3& end, float t );
		HOST_DEVICE FORCEINLINE static Vector3 Reflect( const Vector3& Incident, const Vector3& normal );

		HOST_DEVICE inline float& operator[]( unsigned char i );
		HOST_DEVICE inline float const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const float* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE bool operator!=( const Vector3& other ) const;

		HOST_DEVICE FORCEINLINE Vector3 operator+( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator-( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator-( void ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator*( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator/( const Vector3& other ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator*( const float& value ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator/( const float& value ) const;

		HOST_DEVICE FORCEINLINE Vector3& operator+=( const Vector3& other );
		HOST_DEVICE FORCEINLINE Vector3& operator-=( const Vector3& other );
		HOST_DEVICE FORCEINLINE Vector3& operator*=( const Vector3& other );
		HOST_DEVICE FORCEINLINE Vector3& operator/=( const Vector3& other );
		HOST_DEVICE FORCEINLINE Vector3& operator*=( const float& value );
		HOST_DEVICE FORCEINLINE Vector3& operator/=( const float& value );

		HOST_DEVICE inline friend Vector3 operator*( float value, const Vector3& vector );
		HOST_DEVICE inline friend Vector3 operator/( float value, const Vector3& vector );
	};

	typedef Vector3 Point3;

}

namespace Math
{
	//* Get the angles in degrees in the range of (-180, 180) 
	inline EE::Vector3 NormalizeAngleComponents( EE::Vector3 eulerAngle );

	//* Get the angles in degrees in the range of [0, 360)
	inline EE::Vector3 ClampAngleComponents( EE::Vector3 eulerAngle );
}

#include "Math/Vector3.inl"
