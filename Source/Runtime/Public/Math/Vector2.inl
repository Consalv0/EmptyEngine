#pragma once

#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE TVector2<T>::TVector2()
		: x( 0 ), y( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TVector2<T>::TVector2( const TVector2<T>& vector )
		: x( vector.x ), y( vector.y )
	{
	}

    template <typename T>
	FORCEINLINE TVector2<T>::TVector2( const TVector3<T>& vector )
		: x( vector.x ), y( vector.y )
	{
	}

    template <typename T>
	FORCEINLINE TVector2<T>::TVector2( const TVector4<T>& vector )
		: x( vector.x ), y( vector.y )
	{
	}

    template <typename T>
    template <typename I>
	FORCEINLINE TVector2<T>::TVector2( const TIntVector2<I>& vector )
		: x( (T)vector.x ), y( (T)vector.y )
	{
	}

    template <typename T>
    template <typename I>
	FORCEINLINE TVector2<T>::TVector2( const TIntVector3<I>& vector )
		: x( (T)vector.x ), y( (T)vector.y )
	{
	}

    template <typename T>
	FORCEINLINE TVector2<T>::TVector2( const T& x, const T& y )
		: x( x ), y( y )
	{
	}

    template <typename T>
	FORCEINLINE TVector2<T>::TVector2( const T& value )
		: x( value ), y( value )
	{
	}

    template <typename T>
	inline T TVector2<T>::Magnitude() const
	{
		return std::sqrt( x * x + y * y );
	}

    template <typename T>
	inline T TVector2<T>::MagnitudeSquared() const
	{
		return x * x + y * y;
	}

    template <typename T>
	inline void TVector2<T>::Normalize()
	{
		*this /= Magnitude();
	}

    template <typename T>
	inline TVector2<T> TVector2<T>::Normalized() const
	{
		TVector2<T> result = TVector2<T>( *this );
		return result /= Magnitude();;
	}

    template <typename T>
	FORCEINLINE T TVector2<T>::Cross( const TVector2<T>& other ) const
	{
		return x * other.y - y * other.x;
	}

    template <typename T>
	FORCEINLINE T TVector2<T>::Cross( const TVector2<T>& a, const TVector2<T>& b )
	{
		return a.x * b.y - a.y * b.x;
	}

    template <typename T>
	FORCEINLINE T TVector2<T>::Dot( const TVector2<T>& other ) const
	{
		return x * other.x + y * other.y;
	}

    template <typename T>
	FORCEINLINE T TVector2<T>::Dot( const TVector2<T>& a, const TVector2<T>& b )
	{
		return a.x * b.x + a.y * b.y;
	}

    template <typename T>
	inline TVector2<T> TVector2<T>::Orthogonal( bool polarity ) const
	{
		return polarity ? TVector2( -y, x ) : TVector2( y, -x );
	}

    template <typename T>
	inline TVector2<T> TVector2<T>::Orthonormal( bool polarity ) const
	{
		T length = Magnitude();
		if ( length == 0 )
			return polarity ? TVector2( 0 ) : TVector2( 0 );
		return polarity ? TVector2( -y / length, x / length ) : TVector2( y / length, -x / length );
	}

    template <typename T>
	inline TVector2<T> TVector2<T>::Rotate( TVector2<T>& a, const TVector2<T>& center, T radians ) const
	{
		TVector2 rot;
		a.x -= center.x;
		a.y -= center.y;
		rot.x = round( a.x * cos( radians ) - a.y * sin( radians ) );
		rot.y = round( a.x * sin( radians ) + a.y * cos( radians ) );
		rot.x += center.x;
		rot.y += center.y;
		return rot;
	}

    template <typename T>
	inline const T* TVector2<T>::PointerToValue() const
	{
		return &x;
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::Lerp( const TVector2<T>& start, const TVector2<T>& end, T t )
	{
		return TVector2( (start.x * (1 - t)) + (end.x * t), (start.y * (1 - t)) + (end.y * t) );
	}

    template <typename T>
	inline T& TVector2<T>::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 1, "TVector2 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	inline T const& TVector2<T>::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 1, "TVector2 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE bool TVector2<T>::operator==( const TVector2<T>& other ) const
	{
		return (x == other.x && y == other.y);
	}

    template <typename T>
	inline HOST_DEVICE bool TVector2<T>::operator!() const
	{
		return !x || !y;
	}

    template <typename T>
	FORCEINLINE bool TVector2<T>::operator!=( const TVector2<T>& other ) const
	{
		return (x != other.x || y != other.y);
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator+( const TVector2<T>& other ) const
	{
		return TVector2( x + other.x, y + other.y );
	}
    template <typename T>

	FORCEINLINE TVector2<T> TVector2<T>::operator-( const TVector2<T>& other ) const
	{
		return TVector2( x - other.x, y - other.y );
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator-( void ) const
	{
		return TVector2( -x, -y );
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator*( const TVector2<T>& other ) const
	{
		return TVector2( x * other.x, y * other.y );
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator/( const TVector2<T>& other ) const
	{
		return TVector2( x / other.x, y / other.y );
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator*( const T& value ) const
	{
		return TVector2( x * value, y * value );
	}

    template <typename T>
	FORCEINLINE TVector2<T> TVector2<T>::operator/( const T& value ) const
	{
		if ( value == 0 ) return TVector2();
		return TVector2( x / value, y / value );
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator+=( const TVector2<T>& other )
	{
		x += other.x; y += other.y; return *this;
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator-=( const TVector2<T>& other )
	{
		x -= other.x; y -= other.y; return *this;
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator*=( const TVector2<T>& other )
	{
		x *= other.x; y *= other.y; return *this;
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator/=( const TVector2<T>& other )
	{
		x /= other.x; y /= other.y; return *this;
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator*=( const T& value )
	{
		x *= value;	y *= value; return *this;
	}

    template <typename T>
	FORCEINLINE TVector2<T>& TVector2<T>::operator/=( const T& value )
	{
		if ( value == 0 ) x = y = 0;
		x /= value; y /= value; return *this;
	}
}