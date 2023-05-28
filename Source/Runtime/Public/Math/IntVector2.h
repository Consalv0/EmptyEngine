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
			struct { int x, y; };
			struct { int u, v; };
		};

		HOST_DEVICE FORCEINLINE IntVector2();
		HOST_DEVICE FORCEINLINE IntVector2( const IntVector2& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector2& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector3& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const Vector4& vector );
		HOST_DEVICE FORCEINLINE IntVector2( const int& value );
		HOST_DEVICE FORCEINLINE IntVector2( const int& x, const int& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline int MagnitudeSquared() const;

		HOST_DEVICE FORCEINLINE int Dot( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE static int Dot( const IntVector2& a, const IntVector2& b );
		HOST_DEVICE FORCEINLINE int Cross( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE static int Cross( const IntVector2& a, const IntVector2& b );

		HOST_DEVICE inline Vector2 FloatVector2() const;
		HOST_DEVICE inline int& operator[]( unsigned char i );
		HOST_DEVICE inline int const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const int* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const IntVector2& other );
		HOST_DEVICE FORCEINLINE bool operator!=( const IntVector2& other );

		HOST_DEVICE FORCEINLINE IntVector2 operator+( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator-( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator-( void ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator*( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator/( const IntVector2& other ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator*( const int& value ) const;
		HOST_DEVICE FORCEINLINE IntVector2 operator/( const int& value ) const;

		HOST_DEVICE FORCEINLINE IntVector2& operator+=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator-=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator*=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator/=( const IntVector2& other );
		HOST_DEVICE FORCEINLINE IntVector2& operator*=( const int& value );
		HOST_DEVICE FORCEINLINE IntVector2& operator/=( const int& value );

		HOST_DEVICE inline friend IntVector2 operator*( int value, const IntVector2& vector );
		HOST_DEVICE inline friend IntVector2 operator/( int value, const IntVector2& vector );
	};

	typedef IntVector2 IntPoint2;
}

#include "Math/IntVector2.inl"