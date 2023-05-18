#pragma once

#include <math.h>
#include <stdexcept>

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/IntVector2.h"

namespace EEngine
{
	FORCEINLINE IntVector2::IntVector2()
		: x( 0 ), y( 0 )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const Vector2& vector )
		: x( (int)vector.x ), y( (int)vector.y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const Vector3& vector )
		: x( (int)vector.x ), y( (int)vector.y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const Vector4& vector )
		: x( (int)vector.x ), y( (int)vector.y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const IntVector2& vector )
		: x( vector.x ), y( vector.y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const IntVector3& vector )
		: x( vector.x ), y( vector.y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const int& x, const int& y )
		: x( x ), y( y )
	{
	}

	FORCEINLINE IntVector2::IntVector2( const int& value )
		: x( value ), y( value )
	{
	}

	inline float IntVector2::Magnitude() const
	{
		return sqrtf( x * float( x ) + y * float( y ) );
	}

	inline int IntVector2::MagnitudeSquared() const
	{
		return x * x + y * y;
	}

	FORCEINLINE HOST_DEVICE int IntVector2::Dot( const IntVector2& other ) const
	{
		return (x * other.x) + (y * other.y);
	}

	FORCEINLINE HOST_DEVICE int IntVector2::Cross( const IntVector2& other ) const
	{
		return x * other.y - y * other.x;
	}

	FORCEINLINE HOST_DEVICE int IntVector2::Dot( const IntVector2& a, const IntVector2& b )
	{
		return (a.x * b.x) + (a.y * b.y);
	}

	FORCEINLINE HOST_DEVICE int IntVector2::Cross( const IntVector2& a, const IntVector2& b )
	{
		return a.x * b.y - a.y * b.x;
	}

	inline Vector2 IntVector2::FloatVector2() const
	{
		return Vector2( float( x ), float( y ) );
	}

	inline const int* IntVector2::PointerToValue() const
	{
		return &x;
	}

	inline int& IntVector2::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 1, "IntVector2 index out of bounds" );
		return ((int*)this)[ i ];
	}

	inline int const& IntVector2::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 1, "IntVector2 index out of bounds" );
		return ((int*)this)[ i ];
	}

	FORCEINLINE IntVector2 IntVector2::operator * ( const IntVector2& other ) const
	{
		return IntVector2(
			x * other.x,
			y * other.y
		);
	}

	FORCEINLINE IntVector2 IntVector2::operator/( const IntVector2& other ) const
	{
		return IntVector2(
			x / other.x,
			y / other.y
		);
	}

	FORCEINLINE bool IntVector2::operator==( const IntVector2& other )
	{
		return (x == other.x && y == other.y);
	}

	FORCEINLINE bool IntVector2::operator!=( const IntVector2& other )
	{
		return (x != other.x || y != other.y);
	}

	FORCEINLINE IntVector2 IntVector2::operator+( const IntVector2& other ) const
	{
		return IntVector2( x + other.x, y + other.y );
	}

	FORCEINLINE IntVector2 IntVector2::operator-( const IntVector2& other ) const
	{
		return IntVector2( x - other.x, y - other.y );
	}

	FORCEINLINE IntVector2 IntVector2::operator-( void ) const
	{
		return IntVector2( -x, -y );
	}

	FORCEINLINE IntVector2 IntVector2::operator*( const int& value ) const
	{
		return IntVector2( x * value, y * value );
	}

	FORCEINLINE IntVector2 IntVector2::operator/( const int& value ) const
	{
		if ( value == 0 ) IntVector2();
		return IntVector2( x / value, y / value );
	}

	FORCEINLINE IntVector2& IntVector2::operator+=( const IntVector2& other )
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	FORCEINLINE IntVector2& IntVector2::operator-=( const IntVector2& other )
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	FORCEINLINE IntVector2& IntVector2::operator*=( const IntVector2& other )
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	FORCEINLINE IntVector2& IntVector2::operator/=( const IntVector2& other )
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	FORCEINLINE IntVector2& IntVector2::operator*=( const int& value )
	{
		x *= value;
		y *= value;
		return *this;
	}

	FORCEINLINE IntVector2& IntVector2::operator/=( const int& value )
	{
		if ( value == 0 ) x = y = 0;
		x /= value;
		y /= value;
		return *this;
	}

	inline IntVector2 operator*( int value, const IntVector2& vector )
	{
		return IntVector2( value * vector.x, value * vector.y );
	}

	inline IntVector2 operator/( int value, const IntVector2& vector )
	{
		return IntVector2( value / vector.x, value / vector.y );
	}
}