#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/IntVector3.h"
#include "Math/Vector4.h"

namespace EE
{
	FORCEINLINE Vector4::Vector4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{
	}

	FORCEINLINE Vector4::Vector4( const Vector4& vector )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( vector.w )
	{
	}

	FORCEINLINE Vector4::Vector4( const float& x, const float& y, const float& z )
		: x( x ), y( y ), z( z ), w( 0 )
	{
	}

	FORCEINLINE Vector4::Vector4( const float& x, const float& y, const float& z, const float& w )
		: x( x ), y( y ), z( z ), w( w )
	{
	}

	FORCEINLINE Vector4::Vector4( const Vector2& vector )
		: x( vector.x ), y( vector.y ), z( 0 ), w( 0 )
	{
	}

	FORCEINLINE Vector4::Vector4( const Vector3& vector )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( 0 )
	{
	}

	FORCEINLINE Vector4::Vector4( const Vector3& vector, const float& w )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( w )
	{
	}

	FORCEINLINE Vector4::Vector4( const float& value )
		: x( value ), y( value ), z( value ), w( value )
	{
	}

	inline float Vector4::Magnitude() const
	{
		return sqrtf( x * x + y * y + z * z + w * w );
	}

	inline float Vector4::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline void Vector4::Normalize()
	{
		float sqrMagnitude = MagnitudeSquared();
		if ( sqrMagnitude == 0 )
		{
			x = 0; y = 0; z = 0; w = 0;
		}
		else
		{
			*this /= sqrtf( sqrMagnitude );
		}
	}

	inline Vector4 Vector4::Normalized() const
	{
		float sqrMagnitude = MagnitudeSquared();
		if ( sqrMagnitude == 0 ) return Vector4();
		Vector4 result = Vector4( *this );
		return result /= sqrtf( sqrMagnitude );
	}

	FORCEINLINE float Vector4::Dot( const Vector4& other ) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	FORCEINLINE float Vector4::Dot( const Vector4& a, const Vector4& b )
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	inline const float* Vector4::PointerToValue() const
	{
		return &x;
	}

	FORCEINLINE Vector4 Vector4::Lerp( const Vector4& start, const Vector4& end, float t )
	{
		return Vector4(
			(start.x * (1.0F - t)) + (end.x * t),
			(start.y * (1.0F - t)) + (end.y * t),
			(start.z * (1.0F - t)) + (end.z * t),
			(start.w * (1.0F - t)) + (end.w * t)
		);
	}

	inline float& Vector4::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 3, L"Vector4 index out of bounds" );
		return ((float*)this)[ i ];
	}

	inline float const& Vector4::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 3, L"Vector4 index out of bounds" );
		return ((float*)this)[ i ];
	}

	FORCEINLINE bool Vector4::operator==( const Vector4& other ) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

	FORCEINLINE bool Vector4::operator!=( const Vector4& other ) const
	{
		return (x != other.x || y != other.y || z != other.z || w != other.w);
	}

	FORCEINLINE Vector4 Vector4::operator+( const Vector4& other ) const
	{
		return Vector4( x + other.x, y + other.y, z + other.z, w + other.w );
	}

	FORCEINLINE Vector4 Vector4::operator-( const Vector4& other ) const
	{
		return Vector4( x - other.x, y - other.y, z - other.z, w - other.w );
	}

	FORCEINLINE Vector4 Vector4::operator-( void ) const
	{
		return Vector4( -x, -y, -z, -w );
	}

	FORCEINLINE Vector4 Vector4::operator*( const float& value ) const
	{
		return Vector4( x * value, y * value, z * value, w * value );
	}

	FORCEINLINE Vector4 Vector4::operator/( const float& value ) const
	{
		if ( value == 0 ) return Vector4();
		return Vector4( x / value, y / value, z / value, w / value );
	}

	FORCEINLINE Vector4 Vector4::operator*( const Vector4& other ) const
	{
		return Vector4( x * other.x, y * other.y, z * other.z, w * other.w );
	}

	FORCEINLINE Vector4 Vector4::operator/( const Vector4& other ) const
	{
		return Vector4( x / other.x, y / other.y, z / other.z, w / other.w );
	}

	FORCEINLINE Vector4& Vector4::operator+=( const Vector4& other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	FORCEINLINE Vector4& Vector4::operator-=( const Vector4& other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	FORCEINLINE Vector4& Vector4::operator*=( const Vector4& other )
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	FORCEINLINE Vector4& Vector4::operator/=( const Vector4& other )
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	FORCEINLINE Vector4& Vector4::operator*=( const float& value )
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	FORCEINLINE Vector4& Vector4::operator/=( const float& value )
	{
		if ( value == 0 ) x = y = z = w = 0;
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

	inline Vector4 operator*( float value, const Vector4& vector )
	{
		return Vector4( value * vector.x, value * vector.y, value * vector.z, value * vector.w );
	}

	inline Vector4 operator/( float value, const Vector4& vector )
	{
		return Vector4( value / vector.x, value / vector.y, value / vector.z, value / vector.w );
	}

}