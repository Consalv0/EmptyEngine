#pragma once

#include <cmath>
#include <stdexcept>

#include "IntVector2.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace EE
{
	FORCEINLINE IntVector3::IntVector3()
		: x( 0 ), y( 0 ), z( 0 )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const Vector2& vector )
		: x( (int32)vector.x ), y( (int32)vector.y ), z( 0 )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const Vector3& vector )
		: x( (int32)vector.x ), y( (int32)vector.y ), z( (int32)vector.z )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const IntVector3& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const IntVector2& vector )
		: x( vector.x ), y( vector.y ), z( 0 )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const IntVector2& vector, const int32& z )
		: x( vector.x ), y( vector.y ), z( z )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const Vector4& vector )
		: x( (int32)vector.x ), y( (int32)vector.y ), z( (int32)vector.z )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const int32& x, const int32& y, const int32& z )
		: x( x ), y( y ), z( z )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const int32& x, const int32& y )
		: x( x ), y( y ), z( 0 )
	{
	}

	FORCEINLINE IntVector3::IntVector3( const int32& value )
		: x( value ), y( value ), z( value )
	{
	}

	inline float IntVector3::Magnitude() const
	{
		return sqrtf( x * float( x ) + y * float( y ) + z * float( z ) );
	}

	inline int IntVector3::MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

	FORCEINLINE IntVector3 IntVector3::Cross( const IntVector3& other ) const
	{
		return IntVector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	FORCEINLINE int32 IntVector3::Dot( const IntVector3& other ) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

	inline Vector3 IntVector3::FloatVector3() const
	{
		return Vector3( float( x ), float( y ), float( z ) );
	}

	inline const int32* IntVector3::PointerToValue() const
	{
		return &x;
	}

	inline int32& IntVector3::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "IntVector3 index out of bounds" );
		return ((int32*)this)[ i ];
	}

	inline int32 const& IntVector3::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "IntVector3 index out of bounds" );
		return ((int32*)this)[ i ];
	}

	FORCEINLINE IntVector3 IntVector3::operator * ( const IntVector3& other ) const
	{
		return IntVector3(
			x * other.x,
			y * other.y,
			z * other.z
		);
	}

	FORCEINLINE IntVector3 IntVector3::operator/( const IntVector3& other ) const
	{
		return IntVector3(
			x / other.x,
			y / other.y,
			z / other.z
		);
	}

	FORCEINLINE bool IntVector3::operator==( const IntVector3& other )
	{
		return (x == other.x && y == other.y && z == other.z);
	}

	FORCEINLINE bool IntVector3::operator!=( const IntVector3& other )
	{
		return (x != other.x || y != other.y || z != other.z);
	}

	FORCEINLINE IntVector3 IntVector3::operator+( const IntVector3& other ) const
	{
		return IntVector3( x + other.x, y + other.y, z + other.z );
	}

	FORCEINLINE IntVector3 IntVector3::operator-( const IntVector3& other ) const
	{
		return IntVector3( x - other.x, y - other.y, z - other.z );
	}

	FORCEINLINE IntVector3 IntVector3::operator-( void ) const
	{
		return IntVector3( -x, -y, -z );
	}

	FORCEINLINE IntVector3 IntVector3::operator*( const int32& value ) const
	{
		return IntVector3( x * value, y * value, z * value );
	}

	FORCEINLINE IntVector3 IntVector3::operator/( const int32& value ) const
	{
		if ( value == 0 ) return IntVector3();
		return IntVector3( x / value, y / value, z / value );
	}

	FORCEINLINE IntVector3& IntVector3::operator+=( const IntVector3& other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	FORCEINLINE IntVector3& IntVector3::operator-=( const IntVector3& other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	FORCEINLINE IntVector3& IntVector3::operator*=( const IntVector3& other )
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	FORCEINLINE IntVector3& IntVector3::operator/=( const IntVector3& other )
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	FORCEINLINE IntVector3& IntVector3::operator*=( const int32& value )
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	FORCEINLINE IntVector3& IntVector3::operator/=( const int32& value )
	{
		if ( value == 0 ) x = y = z = 0;
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	inline IntVector3 operator*( int32 value, const IntVector3& vector )
	{
		return IntVector3( value * vector.x, value * vector.y, value / vector.z );
	}

	inline IntVector3 operator/( int32 value, const IntVector3& vector )
	{
		return IntVector3( value / vector.x, value / vector.y, value / vector.z );
	}
}