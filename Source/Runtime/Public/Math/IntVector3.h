#pragma once

#include "CoreTypes.h"

namespace EE
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
			struct { int32 x, y, z; };
			struct { int32 r, g, b; };
		};

		HOST_DEVICE FORCEINLINE IntVector3();
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector3& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector2& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const IntVector2& vector, const int32& z );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector2& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector3& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const Vector4& vector );
		HOST_DEVICE FORCEINLINE IntVector3( const int32& value );
		HOST_DEVICE FORCEINLINE IntVector3( const int32& x, const int32& y, const int32& z );
		HOST_DEVICE FORCEINLINE IntVector3( const int32& x, const int32& y );

		HOST_DEVICE inline float Magnitude() const;
		HOST_DEVICE inline int32 MagnitudeSquared() const;

		HOST_DEVICE FORCEINLINE IntVector3 Cross( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE int32 Dot( const IntVector3& other ) const;

		HOST_DEVICE inline Vector3 FloatVector3() const;
		HOST_DEVICE inline int32& operator[]( unsigned char i );
		HOST_DEVICE inline int32 const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const int32* PointerToValue() const;

		HOST_DEVICE FORCEINLINE bool operator==( const IntVector3& other );
		HOST_DEVICE FORCEINLINE bool operator!=( const IntVector3& other );

		HOST_DEVICE FORCEINLINE IntVector3 operator+( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator-( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator-( void ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator*( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator/( const IntVector3& other ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator*( const int32& value ) const;
		HOST_DEVICE FORCEINLINE IntVector3 operator/( const int32& value ) const;

		HOST_DEVICE FORCEINLINE IntVector3& operator+=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator-=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator*=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator/=( const IntVector3& other );
		HOST_DEVICE FORCEINLINE IntVector3& operator*=( const int32& value );
		HOST_DEVICE FORCEINLINE IntVector3& operator/=( const int32& value );

		HOST_DEVICE inline friend IntVector3 operator*( int32 value, const IntVector3& vector );
		HOST_DEVICE inline friend IntVector3 operator/( int32 value, const IntVector3& vector );
	};

	typedef IntVector3 IntPoint3;

}

#include "Math/IntVector3.inl"

