#pragma once

#include <cmath>
#include <stdexcept>

#include "IntVector2.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3()
		: x( 0 ), y( 0 ), z( 0 )
	{
	}

    template <typename T>
    template <typename R>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TVector2<R>& vector )
		: x( (T)vector.x ), y( (T)vector.y ), z( (T)0 )
	{
	}

    template <typename T>
    template <typename R>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TVector3<R>& vector )
		: x( (T)vector.x ), y( (T)vector.y ), z( (T)vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TIntVector3& vector )
		: x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TIntVector2<T>& vector )
		: x( vector.x ), y( vector.y ), z( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TIntVector2<T>& vector, const T& z )
		: x( vector.x ), y( vector.y ), z( z )
	{
	}

    template <typename T>
    template <typename R>
	FORCEINLINE TIntVector3<T>::TIntVector3( const TVector4<R>& vector )
		: x( (T)vector.x ), y( (T)vector.y ), z( (T)vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const T& x, const T& y, const T& z )
		: x( x ), y( y ), z( z )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const T& x, const T& y )
		: x( x ), y( y ), z( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>::TIntVector3( const T& value )
		: x( value ), y( value ), z( value )
	{
	}

    template <typename T>
    template <typename R>
	inline R TIntVector3<T>::Magnitude() const
	{
		return sqrtf( x * R( x ) + y * R( y ) + z * R( z ) );
	}

    template <typename T>
	inline T TIntVector3<T>::MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::Cross( const TIntVector3& other ) const
	{
		return TIntVector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

    template <typename T>
	FORCEINLINE T TIntVector3<T>::Dot( const TIntVector3& other ) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

    template <typename T>
    template <typename V>
	inline V TIntVector3<T>::ToFloat() const
	{
		return V( x, y, z );
	}

    template <typename T>
	inline const T* TIntVector3<T>::PointerToValue() const
	{
		return &x;
	}

    template <typename T>
	inline T& TIntVector3<T>::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "TIntVector3 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	inline T const& TIntVector3<T>::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "TIntVector3 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator * ( const TIntVector3& other ) const
	{
		return TIntVector3(
			x * other.x,
			y * other.y,
			z * other.z
		);
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator/( const TIntVector3& other ) const
	{
		return TIntVector3(
			x / other.x,
			y / other.y,
			z / other.z
		);
	}

    template <typename T>
	FORCEINLINE bool TIntVector3<T>::operator==( const TIntVector3& other )
	{
		return (x == other.x && y == other.y && z == other.z);
	}

    template <typename T>
	FORCEINLINE bool TIntVector3<T>::operator!=( const TIntVector3& other )
	{
		return (x != other.x || y != other.y || z != other.z);
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator+( const TIntVector3& other ) const
	{
		return TIntVector3( x + other.x, y + other.y, z + other.z );
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator-( const TIntVector3& other ) const
	{
		return TIntVector3( x - other.x, y - other.y, z - other.z );
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator-( void ) const
	{
		return TIntVector3( -x, -y, -z );
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator*( const T& value ) const
	{
		return TIntVector3( x * value, y * value, z * value );
	}

    template <typename T>
	FORCEINLINE TIntVector3<T> TIntVector3<T>::operator/( const T& value ) const
	{
		return TIntVector3( x / value, y / value, z / value );
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator+=( const TIntVector3& other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator-=( const TIntVector3& other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator*=( const TIntVector3& other )
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator/=( const TIntVector3& other )
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator*=( const T& value )
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector3<T>& TIntVector3<T>::operator/=( const T& value )
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

    template <typename T>
	inline TIntVector3<T> operator*( T value, const TIntVector3<T>& vector )
	{
		return TIntVector3( value * vector.x, value * vector.y, value / vector.z );
	}

    template <typename T>
	inline TIntVector3<T> operator/( T value, const TIntVector3<T>& vector )
	{
		return TIntVector3( value / vector.x, value / vector.y, value / vector.z );
	}
}