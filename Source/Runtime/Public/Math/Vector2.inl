#pragma once

#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"

namespace EEngine
{

	FORCEINLINE Vector2::Vector2()
		: x( 0 ), y( 0 )
	{
	}

	FORCEINLINE Vector2::Vector2( const Vector2& vector )
		: x( vector.x ), y( vector.y )
	{
	}

	FORCEINLINE Vector2::Vector2( const Vector3& vector )
		: x( vector.x ), y( vector.y )
	{
	}

	FORCEINLINE Vector2::Vector2( const Vector4& vector )
		: x( vector.x ), y( vector.y )
	{
	}

	FORCEINLINE Vector2::Vector2( const IntVector2& vector )
		: x( (float)vector.x ), y( (float)vector.y )
	{
	}

	FORCEINLINE Vector2::Vector2( const IntVector3& vector )
		: x( (float)vector.x ), y( (float)vector.y )
	{
	}

	FORCEINLINE Vector2::Vector2( const float& x, const float& y )
		: x( x ), y( y )
	{
	}

	FORCEINLINE Vector2::Vector2( const float& value )
		: x( value ), y( value )
	{
	}

	inline float Vector2::Magnitude() const
	{
		return sqrtf( x * x + y * y );
	}

	inline float Vector2::MagnitudeSquared() const
	{
		return x * x + y * y;
	}

	inline void Vector2::Normalize()
	{
		*this /= Magnitude();
	}

	inline Vector2 Vector2::Normalized() const
	{
		Vector2 result = Vector2( *this );
		return result /= Magnitude();;
	}

	FORCEINLINE float Vector2::Cross( const Vector2& other ) const
	{
		return x * other.y - y * other.x;
	}

	FORCEINLINE float Vector2::Cross( const Vector2& a, const Vector2& b )
	{
		return a.x * b.y - a.y * b.x;
	}

	FORCEINLINE float Vector2::Dot( const Vector2& other ) const
	{
		return x * other.x + y * other.y;
	}

	FORCEINLINE float Vector2::Dot( const Vector2& a, const Vector2& b )
	{
		return a.x * b.x + a.y * b.y;
	}

	inline Vector2 Vector2::Orthogonal( bool polarity ) const
	{
		return polarity ? Vector2( -y, x ) : Vector2( y, -x );
	}

	inline Vector2 Vector2::Orthonormal( bool polarity ) const
	{
		float length = Magnitude();
		if ( length == 0 )
			return polarity ? Vector2( 0 ) : Vector2( 0 );
		return polarity ? Vector2( -y / length, x / length ) : Vector2( y / length, -x / length );
	}

	inline Vector2 Vector2::Rotate( Vector2& a, const Vector2& center, float radians ) const
	{
		Vector2 rot;
		a.x -= center.x;
		a.y -= center.y;
		rot.x = round( a.x * cos( radians ) - a.y * sin( radians ) );
		rot.y = round( a.x * sin( radians ) + a.y * cos( radians ) );
		rot.x += center.x;
		rot.y += center.y;
		return rot;
	}

	inline const float* Vector2::PointerToValue() const
	{
		return &x;
	}

	FORCEINLINE Vector2 Vector2::Lerp( const Vector2& start, const Vector2& end, float t )
	{
		return Vector2( (start.x * (1.0F - t)) + (end.x * t), (start.y * (1.0F - t)) + (end.y * t) );
	}

	inline float& Vector2::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 1, "Vector2 index out of bounds" );
		return ((float*)this)[ i ];
	}

	inline float const& Vector2::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 1, "Vector2 index out of bounds" );
		return ((float*)this)[ i ];
	}

	FORCEINLINE bool Vector2::operator==( const Vector2& other ) const
	{
		return (x == other.x && y == other.y);
	}

	inline HOST_DEVICE bool Vector2::operator!() const
	{
		return !x || !y;
	}

	FORCEINLINE bool Vector2::operator!=( const Vector2& other ) const
	{
		return (x != other.x || y != other.y);
	}

	FORCEINLINE Vector2 Vector2::operator+( const Vector2& other ) const
	{
		return Vector2( x + other.x, y + other.y );
	}

	FORCEINLINE Vector2 Vector2::operator-( const Vector2& other ) const
	{
		return Vector2( x - other.x, y - other.y );
	}

	FORCEINLINE Vector2 Vector2::operator-( void ) const
	{
		return Vector2( -x, -y );
	}

	FORCEINLINE Vector2 Vector2::operator*( const Vector2& other ) const
	{
		return Vector2( x * other.x, y * other.y );
	}

	FORCEINLINE Vector2 Vector2::operator/( const Vector2& other ) const
	{
		return Vector2( x / other.x, y / other.y );
	}

	FORCEINLINE Vector2 Vector2::operator*( const float& value ) const
	{
		return Vector2( x * value, y * value );
	}

	FORCEINLINE Vector2 Vector2::operator/( const float& value ) const
	{
		if ( value == 0 ) return Vector2();
		return Vector2( x / value, y / value );
	}

	FORCEINLINE Vector2& Vector2::operator+=( const Vector2& other )
	{
		x += other.x; y += other.y; return *this;
	}

	FORCEINLINE Vector2& Vector2::operator-=( const Vector2& other )
	{
		x -= other.x; y -= other.y; return *this;
	}

	FORCEINLINE Vector2& Vector2::operator*=( const Vector2& other )
	{
		x *= other.x; y *= other.y; return *this;
	}

	FORCEINLINE Vector2& Vector2::operator/=( const Vector2& other )
	{
		x /= other.x; y /= other.y; return *this;
	}

	FORCEINLINE Vector2& Vector2::operator*=( const float& value )
	{
		x *= value;	y *= value; return *this;
	}

	FORCEINLINE Vector2& Vector2::operator/=( const float& value )
	{
		if ( value == 0 ) x = y = 0;
		x /= value; y /= value; return *this;
	}

	inline Vector2 operator*( float value, const Vector2& vector )
	{
		return Vector2( value * vector.x, value * vector.y );
	}

	inline Vector2 operator/( float value, const Vector2& vector )
	{
		return Vector2( value / vector.x, value / vector.y );
	}

}