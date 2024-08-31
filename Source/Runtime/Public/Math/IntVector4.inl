
namespace EE::Math
{
    template <typename T>
	FORCEINLINE constexpr TIntVector4<T>::TIntVector4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{
	}

    template <typename T>
    template <typename R>
	FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TVector2<R>& vector )
		: x( (T)vector.x ), y( (T)vector.y ), z( (T)0 ), w( (T)0 )
	{
	}

    template <typename T>
    template <typename R>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TVector3<R>& vector )
        : x( (T)vector.x ), y( (T)vector.y ), z( (T)vector.z ), w( (T)0 )
    {
    }

    template <typename T>
    template <typename R>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TVector4<R>& vector )
        : x( (T)vector.x ), y( (T)vector.y ), z( (T)vector.z ), w( (T)vector.w )
    {
    }

    template <typename T>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TIntVector2<T>& vector )
        : x( vector.x ), y( vector.y ), z( 0 ), w( 0 )
    {
    }

    template <typename T>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TIntVector3<T>& vector )
        : x( vector.x ), y( vector.y ), z( vector.z ), w( 0 )
    {
    }

    template <typename T>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const TIntVector3<T>& vector, const T& w )
        : x( vector.x ), y( vector.y ), z( vector.z ), w( (T)w )
    {
    }

    template <typename T>
    FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const T& x, const T& y, const T& z, const T& w )
        : x( x ), y( y ), z( z ), w( w )
    {
    }

    template <typename T>
	FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const T& x, const T& y, const T& z )
		: x( x ), y( y ), z( z ), w( 0 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr  TIntVector4<T>::TIntVector4( const T& x, const T& y )
		: x( x ), y( y ), z( 0 ), w( 0 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TIntVector4<T>::TIntVector4( const T& value )
		: x( value ), y( value ), z( value ), w( value )
	{
	}

    template <typename T>
    template <typename R>
	inline R TIntVector4<T>::Magnitude() const
	{
		return sqrtf( x * R( x ) + y * R( y ) + z * R( z ) + w * R( w ) );
	}

    template <typename T>
	inline T TIntVector4<T>::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

    template <typename T>
	FORCEINLINE T TIntVector4<T>::Dot( const TIntVector4& other ) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
	}

    template <typename T>
    template <typename V>
	inline V TIntVector4<T>::ToFloat() const
	{
		return V( x, y, z, w );
	}

    template <typename T>
	inline const T* TIntVector4<T>::PointerToValue() const
	{
		return &x;
	}

    template <typename T>
	inline T& TIntVector4<T>::operator[]( unsigned char i )
	{
		EE_ASSERT( i <= 3, "TIntVector4 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	inline T const& TIntVector4<T>::operator[]( unsigned char i ) const
	{
		EE_ASSERT( i <= 3, "TIntVector4 index out of bounds" );
		return ((T*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator * ( const TIntVector4& other ) const
	{
		return TIntVector4(
			x * other.x,
			y * other.y,
			z * other.z,
            w * other.w
		);
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator/( const TIntVector4& other ) const
	{
		return TIntVector4(
			x / other.x,
			y / other.y,
			z / other.z,
            w / other.w
		);
	}

    template <typename T>
	FORCEINLINE bool TIntVector4<T>::operator==( const TIntVector4& other )
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

    template <typename T>
	FORCEINLINE bool TIntVector4<T>::operator!=( const TIntVector4& other )
	{
		return (x != other.x || y != other.y || z != other.z || w != other.z);
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator+( const TIntVector4& other ) const
	{
		return TIntVector4( x + other.x, y + other.y, z + other.z, w + other.w );
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator-( const TIntVector4& other ) const
	{
		return TIntVector4( x - other.x, y - other.y, z - other.z, w - other.w );
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator-( void ) const
	{
		return TIntVector4( -x, -y, -z, -w );
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator*( const T& value ) const
	{
		return TIntVector4( x * value, y * value, z * value, w * value );
	}

    template <typename T>
	FORCEINLINE TIntVector4<T> TIntVector4<T>::operator/( const T& value ) const
	{
		return TIntVector4( x / value, y / value, z / value, w / value );
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator+=( const TIntVector4& other )
	{
		x += other.x;
		y += other.y;
        z += other.z;
        w += other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator-=( const TIntVector4& other )
	{
		x -= other.x;
		y -= other.y;
        z -= other.z;
        w -= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator*=( const TIntVector4& other )
	{
		x *= other.x;
		y *= other.y;
        z *= other.z;
        w *= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator/=( const TIntVector4& other )
	{
		x /= other.x;
		y /= other.y;
        z /= other.z;
        w /= other.w;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator*=( const T& value )
	{
		x *= value;
		y *= value;
        z *= value;
        w *= value;
		return *this;
	}

    template <typename T>
	FORCEINLINE TIntVector4<T>& TIntVector4<T>::operator/=( const T& value )
	{
		x /= value;
		y /= value;
        z /= value;
        w /= value;
		return *this;
	}

    template <typename T>
	inline TIntVector4<T> operator*( T value, const TIntVector4<T>& vector )
	{
        return TIntVector4( value * vector.x, value * vector.y, value / vector.z, value / vector.w );
	}

    template <typename T>
	inline TIntVector4<T> operator/( T value, const TIntVector4<T>& vector )
	{
		return TIntVector3( value / vector.x, value / vector.y, value / vector.z, value / vector.w );
	}
}