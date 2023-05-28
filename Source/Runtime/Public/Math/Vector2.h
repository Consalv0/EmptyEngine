#pragma once

#include "CoreTypes.h"

namespace EE
{
	struct Vector3;
	struct Vector4;
	struct IntVector2;
	struct IntVector3;

	struct Vector2
	{
	public:
		union
		{
			struct { float x, y; };
			struct { float u, v; };
		};

		HOST_DEVICE FORCEINLINE Vector2();
		HOST_DEVICE FORCEINLINE Vector2( const Vector2& vector );
		HOST_DEVICE FORCEINLINE Vector2( const Vector3& vector );
		HOST_DEVICE FORCEINLINE Vector2( const Vector4& vector );
		HOST_DEVICE FORCEINLINE Vector2( const IntVector2& vector );
		HOST_DEVICE FORCEINLINE Vector2( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE Vector2( const float& value );
		HOST_DEVICE FORCEINLINE Vector2( const float& x, const float& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline float MagnitudeSquared() const;
		HOST_DEVICE inline void Normalize();
		HOST_DEVICE inline Vector2 Normalized() const;

		//* a special version of the cross product for 2D vectors (returns scalar value).
		HOST_DEVICE FORCEINLINE float Cross( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE static float Cross( const Vector2& a, const Vector2& b );
		HOST_DEVICE FORCEINLINE float Dot( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE static float Dot( const Vector2& a, const Vector2& b );
		HOST_DEVICE FORCEINLINE Vector2 Orthogonal( bool polarity ) const;
		HOST_DEVICE FORCEINLINE Vector2 Orthonormal( bool polarity ) const;
		HOST_DEVICE FORCEINLINE static Vector2 Lerp( const Vector2& start, const Vector2& end, float t );

		HOST_DEVICE FORCEINLINE Vector2 Rotate( Vector2& a, const Vector2& center, float radians ) const;

		HOST_DEVICE inline float& operator[]( unsigned char i );
		HOST_DEVICE inline float const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const float* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE bool operator!() const;
		HOST_DEVICE FORCEINLINE bool operator!=( const Vector2& other ) const;

		HOST_DEVICE FORCEINLINE Vector2 operator+( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator-( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator-( void ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator*( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator/( const Vector2& other ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator*( const float& value ) const;
		HOST_DEVICE FORCEINLINE Vector2 operator/( const float& value ) const;

		HOST_DEVICE FORCEINLINE Vector2& operator+=( const Vector2& other );
		HOST_DEVICE FORCEINLINE Vector2& operator-=( const Vector2& other );
		HOST_DEVICE FORCEINLINE Vector2& operator*=( const Vector2& other );
		HOST_DEVICE FORCEINLINE Vector2& operator/=( const Vector2& other );
		HOST_DEVICE FORCEINLINE Vector2& operator*=( const float& value );
		HOST_DEVICE FORCEINLINE Vector2& operator/=( const float& value );

		HOST_DEVICE inline friend Vector2 operator*( float value, const Vector2& vector );
		HOST_DEVICE inline friend Vector2 operator/( float value, const Vector2& vector );
	};

	typedef Vector2 Point2;
}

#include "Math/Vector2.inl"

