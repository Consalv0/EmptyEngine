#pragma once

#include "CoreTypes.h"

namespace EE
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct IntVector3;

	struct IntVector2
	{
	public:
		union
		{
			struct { int32 x, y; };
			struct { int32 u, v; };
		};

		HOST_DEVICE FORCEINLINE IntVector2();
		HOST_DEVICE FORCEINLINE IntVector2( const IntVector2& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector2& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector3& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector4& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const int32& value );
		HOST_DEVICE FORCEINLINE IntVector2( const int32& x, const int32& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline int32 MagnitudeSquared() const;

		HOST_DEVICE FORCEINLINE int32 Dot( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE static int32 Dot( const IntVector2& a, const IntVector2& b );
		HOST_DEVICE FORCEINLINE int32 Cross( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE static int32 Cross( const IntVector2& a, const IntVector2& b );

		HOST_DEVICE inline Vector2 FloatVector2() const;
		HOST_DEVICE inline int32& operator[]( unsigned char i );
		HOST_DEVICE inline int32 const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const int32* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const IntVector2& other );
		HOST_DEVICE FORCEINLINE bool operator!=( const IntVector2& other );

		HOST_DEVICE FORCEINLINE IntVector2 operator+( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator-( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator-( void ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator*( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator/( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator*( const int32& value ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator/( const int32& value ) const;

		HOST_DEVICE FORCEINLINE IntVector2& operator+=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator-=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator*=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator/=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator*=( const int32& value );
		HOST_DEVICE FORCEINLINE IntVector2& operator/=( const int32& value );

		HOST_DEVICE inline friend IntVector2 operator*( int32 value, const IntVector2& vector );
		HOST_DEVICE inline friend IntVector2 operator/( int32 value, const IntVector2& vector );
	};

	typedef IntVector2 IntPoint2;
}

#include "Math/IntVector2.inl"