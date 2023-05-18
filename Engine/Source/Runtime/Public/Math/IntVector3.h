#pragma once

#include "CoreTypes.h"

namespace EEngine
{
	struct IntVector2;
	struct Vector2;
	struct Vector3;
	struct Vector4;

	struct IntVector3
	{
	public:
		union
		{
			struct { int x, y, z; };
			struct { int r, g, b; };
		};

		HOST_DEVICE FORCEINLINE IntVector3();
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector2& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector2& vector, const int& z );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector2& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector3& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector4& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const int& value );
		HOST_DEVICE FORCEINLINE IntVector3( const int& x, const int& y, const int& z );
		HOST_DEVICE FORCEINLINE IntVector3( const int& x, const int& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline int MagnitudeSquared() const;

		HOST_DEVICE FORCEINLINE IntVector3 Cross( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE int Dot( const IntVector3& other ) const;

		HOST_DEVICE inline Vector3 FloatVector3() const;
		HOST_DEVICE inline int& operator[]( unsigned char i );
		HOST_DEVICE inline int const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const int* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const IntVector3& other );
		HOST_DEVICE FORCEINLINE bool operator!=( const IntVector3& other );

		HOST_DEVICE FORCEINLINE IntVector3 operator+( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator-( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator-( void ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator*( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator/( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator*( const int& value ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator/( const int& value ) const;

		HOST_DEVICE FORCEINLINE IntVector3& operator+=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator-=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator*=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator/=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator*=( const int& value );
		HOST_DEVICE FORCEINLINE IntVector3& operator/=( const int& value );

		HOST_DEVICE inline friend IntVector3 operator*( int value, const IntVector3& vector );
		HOST_DEVICE inline friend IntVector3 operator/( int value, const IntVector3& vector );
	};

	typedef IntVector3 IntPoint3;

}

#include "Math/IntVector3.inl"

