#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/IntVector3.h"
#include "Math/Vector4.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{
	}
    template <typename T>

	FORCEINLINE constexpr TVector4<T>::TVector4( const TVector4<T>& vector )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( vector.w )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const T& x, const T& y, const T& z )
		: x( x ), y( y ), z( z ), w( 0 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const T& x, const T& y, const T& z, const T& w )
		: x( x ), y( y ), z( z ), w( w )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const TVector2<T>& vector )
		: x( vector.x ), y( vector.y ), z( 0 ), w( 0 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const TVector3<T>& vector )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( 0 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const TVector3<T>& vector, const T& w )
		: x( vector.x ), y( vector.y ), z( vector.z ), w( w )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TVector4<T>::TVector4( const T& value )
		: x( value ), y( value ), z( value ), w( value )
	{
	}

    template <typename T>
	inline T TVector4<T>::Magnitude() const
	{
		return Math::Sqrt( x * x + y * y + z * z + w * w );
	}

    template <typename T>
	inline T TVector4<T>::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

    template <typename T>
	inline void TVector4<T>::Normalize()
	{
		T sqrMagnitude = MagnitudeSquared();
		if ( sqrMagnitude == T(0) )
		{
			x = T(0); y = T(0); z = T(0); w = T(0);
		}
		else
		{
			*this /= Math::Sqrt( sqrMagnitude );
		}
	}

    template <typename T>
	inline TVector4<T> TVector4<T>::Normalized() const
	{
		T sqrMagnitude = MagnitudeSquared();
		if ( sqrMagnitude == 0 ) return TVector4();
		TVector4 result = TVector4( *this );
		return result /= Math::Sqrt( sqrMagnitude );
	}

    template <typename T>
	FORCEINLINE T TVector4<T>::Dot( const TVector4<T>& other ) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

    template <typename T>
	FORCEINLINE T TVector4<T>::Dot( const TVector4<T>& a, const TVector4<T>& b )
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

    template <typename T>
	inline const T* TVector4<T>::PointerToValue() const
	{
		return &x;
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::Lerp( const TVector4<T>& start, const TVector4<T>& end, T t )
	{
		return TVector4(
			(start.x * (T(1) - t)) + (end.x * t),
			(start.y * (T(1) - t)) + (end.y * t),
			(start.z * (T(1) - t)) + (end.z * t),
			(start.w * (T(1) - t)) + (end.w * t)
		);
	}

    template <typename T>
	inline T& TVector4<T>::operator[]( unsigned char i )
	{
		EE_ASSERT( i <= 3, L"TVector4 index out of bounds" );
		return ((float*)this)[ i ];
	}

    template <typename T>
	inline T const& TVector4<T>::operator[]( unsigned char i ) const
	{
		EE_ASSERT( i <= 3, L"TVector4 index out of bounds" );
		return ((float*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE bool TVector4<T>::operator==( const TVector4<T>& other ) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

    template <typename T>
	FORCEINLINE bool TVector4<T>::operator!=( const TVector4<T>& other ) const
	{
		return (x != other.x || y != other.y || z != other.z || w != other.w);
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator+( const TVector4<T>& other ) const
	{
		return TVector4( x + other.x, y + other.y, z + other.z, w + other.w );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator-( const TVector4<T>& other ) const
	{
		return TVector4( x - other.x, y - other.y, z - other.z, w - other.w );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator-( void ) const
	{
		return TVector4( -x, -y, -z, -w );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator*( const T& value ) const
	{
		return TVector4( x * value, y * value, z * value, w * value );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator/( const T& value ) const
	{
		return TVector4( x / value, y / value, z / value, w / value );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator*( const TVector4<T>& other ) const
	{
		return TVector4( x * other.x, y * other.y, z * other.z, w * other.w );
	}

    template <typename T>
	FORCEINLINE TVector4<T> TVector4<T>::operator/( const TVector4<T>& other ) const
	{
		return TVector4( x / other.x, y / other.y, z / other.z, w / other.w );
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator+=( const TVector4<T>& other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator-=( const TVector4<T>& other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator*=( const TVector4<T>& other )
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator/=( const TVector4<T>& other )
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator*=( const T& value )
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

    template <typename T>
	FORCEINLINE TVector4<T>& TVector4<T>::operator/=( const T& value )
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

    template <typename T, typename R>
	inline TVector4<T> operator*( R value, const TVector4<T>& vector )
	{
		return TVector4( value * vector.x, value * vector.y, value * vector.z, value * vector.w );
	}

    template <typename T, typename R>
	inline TVector4<T> operator/( R value, const TVector4<T>& vector )
	{
		return TVector4( value / vector.x, value / vector.y, value / vector.z, value / vector.w );
	}
}