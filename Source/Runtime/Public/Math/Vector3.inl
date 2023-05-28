#pragma once

#include <math.h>
#include <stdexcept>
#include <string>

#include "Math/Vector2.h"
#include "Math/IntVector3.h"
#include "Math/Vector4.h"

namespace EE
{
	FORCEINLINE Vector3::Vector3()
		: x( 0 ), y( 0 ), z( 0 )
	{
	}

	FORCEINLINE Vector3::Vector3( const Vector2& vector )
		: x( vector.x ), y( vector.y ), z( 0 )
	{
	}

	FORCEINLINE Vector3::Vector3( const IntVector3& vector )
		: x( float( vector.x ) ), y( float( vector.y ) ), z( float( vector.z ) )
	{
	}

	FORCEINLINE Vector3::Vector3( const Vector3& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

	FORCEINLINE Vector3::Vector3( const Vector4& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

	FORCEINLINE Vector3::Vector3( const float& x, const float& y, const float& z )
		: x( x ), y( y ), z( z )
	{
	}

	FORCEINLINE Vector3::Vector3( const float& x, const float& y )
		: x( x ), y( y ), z( 0 )
	{
	}

	FORCEINLINE Vector3::Vector3( const float& value )
		: x( value ), y( value ), z( value )
	{
	}

	inline float Vector3::Magnitude() const
	{
		return sqrtf( x * x + y * y + z * z );
	}

	inline float Vector3::MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

	inline void Vector3::Normalize()
	{
		if ( MagnitudeSquared() == 0 )
		{
			x = 0; y = 0; z = 0;
		}
		else
		{
			*this /= Magnitude();
		}
	}

	inline Vector3 Vector3::Normalized() const
	{
		if ( MagnitudeSquared() == 0 ) return Vector3();
		return *this / Magnitude();
	}

	FORCEINLINE Vector3 Vector3::Cross( const Vector3& other ) const
	{
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	inline Vector3 Vector3::Cross( const Vector3& a, const Vector3& b )
	{
		return Vector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	FORCEINLINE float Vector3::Dot( const Vector3& other ) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

	inline float Vector3::Dot( const Vector3& a, const Vector3& b )
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	inline const float* Vector3::PointerToValue() const
	{
		return &x;
	}

	FORCEINLINE Vector3 Vector3::Lerp( const Vector3& start, const Vector3& end, float t )
	{
		return Vector3( (start.x * (1.0F - t)) + (end.x * t), (start.y * (1.0F - t)) + (end.y * t), (start.z * (1.0F - t)) + (end.z * t) );
	}

	inline HOST_DEVICE Vector3 Vector3::Reflect( const Vector3& Incident, const Vector3& Normal )
	{
		return Incident - (Normal * Normal.Dot( Incident )) * 2.F;
	}

	inline float& Vector3::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "Vector3 index out of bounds" );
		return ((float*)this)[ i ];
	}

	inline float const& Vector3::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "Vector3 index out of bounds" );
		return ((float*)this)[ i ];
	}

	FORCEINLINE Vector3 Vector3::operator * ( const Vector3& other ) const
	{
		return Vector3( x * other.x, y * other.y, z * other.z );
	}

	FORCEINLINE Vector3 Vector3::operator/( const Vector3& other ) const
	{
		return Vector3( x / other.x, y / other.y, z / other.z );
	}

	FORCEINLINE bool Vector3::operator==( const Vector3& other ) const
	{
		return (x == other.x && y == other.y && z == other.z);
	}

	FORCEINLINE bool Vector3::operator!=( const Vector3& other ) const
	{
		return (x != other.x || y != other.y || z != other.z);
	}

	FORCEINLINE Vector3 Vector3::operator+( const Vector3& other ) const
	{
		return Vector3( x + other.x, y + other.y, z + other.z );
	}

	FORCEINLINE Vector3 Vector3::operator-( const Vector3& other ) const
	{
		return Vector3( x - other.x, y - other.y, z - other.z );
	}

	FORCEINLINE Vector3 Vector3::operator-( void ) const
	{
		return Vector3( -x, -y, -z );
	}

	FORCEINLINE Vector3 Vector3::operator*( const float& value ) const
	{
		return Vector3( x * value, y * value, z * value );
	}

	FORCEINLINE Vector3 Vector3::operator/( const float& value ) const
	{
		if ( value == 0 ) return Vector3();
		return Vector3( x / value, y / value, z / value );
	}

	FORCEINLINE Vector3& Vector3::operator+=( const Vector3& other )
	{
		x += other.x; y += other.y;	z += other.z;
		return *this;
	}

	FORCEINLINE Vector3& Vector3::operator-=( const Vector3& other )
	{
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}

	FORCEINLINE Vector3& Vector3::operator*=( const Vector3& other )
	{
		x *= other.x; y *= other.y; z *= other.z;
		return *this;
	}

	FORCEINLINE Vector3& Vector3::operator/=( const Vector3& other )
	{
		x /= other.x; y /= other.y; z /= other.z;
		return *this;
	}

	FORCEINLINE Vector3& Vector3::operator*=( const float& value )
	{
		x *= value; y *= value; z *= value;
		return *this;
	}

	FORCEINLINE Vector3& Vector3::operator/=( const float& value )
	{
		if ( value == 0 ) x = y = z = 0;
		x /= value; y /= value; z /= value;
		return *this;
	}

	inline Vector3 operator*( float value, const Vector3& vector )
	{
		return Vector3( value * vector.x, value * vector.y, value * vector.z );
	}

	inline Vector3 operator/( float value, const Vector3& vector )
	{
		return Vector3( value / vector.x, value / vector.y, value / vector.z );
	}

}

inline EE::Vector3 Math::NormalizeAngleComponents( EE::Vector3 eulerAngle )
{
	eulerAngle.x = NormalizeAngle( eulerAngle.x );
	eulerAngle.y = NormalizeAngle( eulerAngle.y );
	eulerAngle.z = NormalizeAngle( eulerAngle.z );

	return eulerAngle;
}

inline EE::Vector3 Math::ClampAngleComponents( EE::Vector3 eulerAngle )
{
	eulerAngle.x = ClampDegrees( eulerAngle.x );
	eulerAngle.y = ClampDegrees( eulerAngle.y );
	eulerAngle.z = ClampDegrees( eulerAngle.z );

	return eulerAngle;
}