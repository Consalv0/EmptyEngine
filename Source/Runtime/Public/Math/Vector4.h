#pragma once

#include "CoreTypes.h"

namespace EE
{
	struct Vector2;
	struct Vector3;

	struct Vector4
	{
	public:
		union
		{
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
		};

		HOST_DEVICE FORCEINLINE Vector4();
		HOST_DEVICE FORCEINLINE Vector4( const Vector2& vector );
		HOST_DEVICE FORCEINLINE Vector4( const Vector3& vector );
		HOST_DEVICE FORCEINLINE Vector4( const Vector3& vector, const float& w );
		HOST_DEVICE FORCEINLINE Vector4( const Vector4& vector );
		HOST_DEVICE FORCEINLINE Vector4( const float& value );
		HOST_DEVICE FORCEINLINE Vector4( const float& x, const float& y, const float& z );
		HOST_DEVICE FORCEINLINE Vector4( const float& x, const float& y, const float& z, const float& w );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline float MagnitudeSquared() const;
		HOST_DEVICE inline void Normalize();
		HOST_DEVICE inline Vector4 Normalized() const;

		HOST_DEVICE FORCEINLINE float Dot( const Vector4& other ) const;
		HOST_DEVICE FORCEINLINE static float Dot( const Vector4& a, const Vector4& b );
		HOST_DEVICE FORCEINLINE static Vector4 Lerp( const Vector4& start, const Vector4& end, float t );

		HOST_DEVICE inline float& operator[]( unsigned char i );
		HOST_DEVICE inline float const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const float* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const Vector4& other ) const;
		HOST_DEVICE FORCEINLINE bool operator!=( const Vector4& other ) const;

		HOST_DEVICE FORCEINLINE Vector4 operator+( const Vector4& other ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator-( const Vector4& other ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator-( void ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator*( const float& value ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator/( const float& value ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator*( const Vector4& other ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator/( const Vector4& other ) const;

		HOST_DEVICE FORCEINLINE Vector4& operator+=( const Vector4& other );
		HOST_DEVICE FORCEINLINE Vector4& operator-=( const Vector4& other );
		HOST_DEVICE FORCEINLINE Vector4& operator*=( const Vector4& other );
		HOST_DEVICE FORCEINLINE Vector4& operator/=( const Vector4& other );
		HOST_DEVICE FORCEINLINE Vector4& operator*=( const float& value );
		HOST_DEVICE FORCEINLINE Vector4& operator/=( const float& value );

		HOST_DEVICE inline friend Vector4 operator*( float value, const Vector4& vector );
		HOST_DEVICE inline friend Vector4 operator/( float value, const Vector4& vector );
	};
}

#include "../Math/Vector4.inl"
