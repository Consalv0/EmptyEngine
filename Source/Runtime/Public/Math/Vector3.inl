#pragma once

#include <math.h>
#include <stdexcept>
#include <string>

#include "Math/Vector2.h"
#include "Math/IntVector3.h"
#include "Math/Vector4.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE TVector3<T>::TVector3()
		: x( 0 ), y( 0 ), z( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const TVector2<T>& vector )
		: x( vector.x ), y( vector.y ), z( 0 )
	{
	}

    template <typename T>
    template <typename I>
	FORCEINLINE TVector3<T>::TVector3( const TIntVector3<I>& vector )
		: x( T( vector.x ) ), y( T( vector.y ) ), z( T( vector.z ) )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const TVector3<T>& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const TVector4<T>& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const T& x, const T& y, const T& z )
		: x( x ), y( y ), z( z )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const T& x, const T& y )
		: x( x ), y( y ), z( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TVector3<T>::TVector3( const T& value )
		: x( value ), y( value ), z( value )
	{
	}

    template <typename T>
	inline T TVector3<T>::Magnitude() const
	{
		return std::sqrt( x * x + y * y + z * z );
	}

    template <typename T>
	inline T TVector3<T>::MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

    template <typename T>
	inline void TVector3<T>::Normalize()
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

    template <typename T>
	inline TVector3<T> TVector3<T>::Normalized() const
	{
		if ( MagnitudeSquared() == 0 ) return TVector3<T>();
		return *this / Magnitude();
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::Cross( const TVector3<T>& other ) const
	{
		return TVector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

    template <typename T>
	inline TVector3<T> TVector3<T>::Cross( const TVector3<T>& a, const TVector3<T>& b )
	{
		return TVector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

    template <typename T>
	FORCEINLINE T TVector3<T>::Dot( const TVector3<T>& other ) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

    template <typename T>
	inline T TVector3<T>::Dot( const TVector3<T>& a, const TVector3<T>& b )
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

    template <typename T>
	inline const T* TVector3<T>::PointerToValue() const
	{
		return &x;
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::Lerp( const TVector3<T>& start, const TVector3<T>& end, T t )
	{
		return TVector3( (start.x * (1 - t)) + (end.x * t), (start.y * (1 - t)) + (end.y * t), (start.z * (1 - t)) + (end.z * t) );
	}

    template <typename T>
	inline HOST_DEVICE TVector3<T> TVector3<T>::Reflect( const TVector3<T>& Incident, const TVector3<T>& normal )
	{
		return Incident - (normal * normal.Dot( Incident )) * 2;
	}

    template <typename T>
	inline T& TVector3<T>::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "TVector3 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	inline T const& TVector3<T>::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "TVector3 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator * ( const TVector3<T>& other ) const
	{
		return TVector3( x * other.x, y * other.y, z * other.z );
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator/( const TVector3<T>& other ) const
	{
		return TVector3( x / other.x, y / other.y, z / other.z );
	}

    template <typename T>
	FORCEINLINE bool TVector3<T>::operator==( const TVector3<T>& other ) const
	{
		return (x == other.x && y == other.y && z == other.z);
	}

    template <typename T>
	FORCEINLINE bool TVector3<T>::operator!=( const TVector3<T>& other ) const
	{
		return (x != other.x || y != other.y || z != other.z);
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator+( const TVector3<T>& other ) const
	{
		return TVector3( x + other.x, y + other.y, z + other.z );
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator-( const TVector3<T>& other ) const
	{
		return TVector3( x - other.x, y - other.y, z - other.z );
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator-( void ) const
	{
		return TVector3( -x, -y, -z );
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator*( const T& value ) const
	{
		return TVector3( x * value, y * value, z * value );
	}

    template <typename T>
	FORCEINLINE TVector3<T> TVector3<T>::operator/( const T& value ) const
	{
		if ( value == 0 ) return TVector3<T>();
		return TVector3( x / value, y / value, z / value );
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator+=( const TVector3<T>& other )
	{
		x += other.x; y += other.y;	z += other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator-=( const TVector3<T>& other )
	{
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator*=( const TVector3<T>& other )
	{
		x *= other.x; y *= other.y; z *= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator/=( const TVector3<T>& other )
	{
		x /= other.x; y /= other.y; z /= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator*=( const T& value )
	{
		x *= value; y *= value; z *= value;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector3<T>& TVector3<T>::operator/=( const T& value )
	{
		if ( value == 0 ) x = y = z = 0;
		x /= value; y /= value; z /= value;
		return *this;
	}

    template <typename T>
    FORCEINLINE TVector3<T> operator*( T value, const TVector3<T>& vector )
	{
		return TVector3<T>( value * vector.x, value * vector.y, value * vector.z );
	}

    template <typename T>
    FORCEINLINE TVector3<T> operator/( T value, const TVector3<T>& vector )
	{
		return TVector3<T>( value / vector.x, value / vector.y, value / vector.z );
	}

    template <typename T>
    inline TVector3<T> NormalizeAngleComponents( TVector3<T> eulerAngle )
    {
        eulerAngle.x = NormalizeDegrees( eulerAngle.x );
        eulerAngle.y = NormalizeDegrees( eulerAngle.y );
        eulerAngle.z = NormalizeDegrees( eulerAngle.z );

        return eulerAngle;
    }

    template <typename T>
    inline TVector3<T> ClampAngleComponents( TVector3<T> eulerAngle )
    {
        eulerAngle.x = ClampDegrees( eulerAngle.x );
        eulerAngle.y = ClampDegrees( eulerAngle.y );
        eulerAngle.z = ClampDegrees( eulerAngle.z );

        return eulerAngle;
    }
}