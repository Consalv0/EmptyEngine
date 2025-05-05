
namespace EE::Math
{
    template <typename T>
	FORCEINLINE constexpr TMatrix3x3<T>::TMatrix3x3()
	{
		m00 = 1; m01 = 0; m02 = 0;
		m10 = 0; m11 = 1; m12 = 0;
		m20 = 0; m21 = 0; m22 = 1;
	}

    template <typename T>
	FORCEINLINE constexpr TMatrix3x3<T>::TMatrix3x3( const TMatrix3x3& matrix )
		: c0( matrix.c0 ), c1( matrix.c1 ), c2( matrix.c2 )
	{
	}

    template <typename T>
	FORCEINLINE constexpr TMatrix3x3<T>::TMatrix3x3( const TMatrix4x4<T>& matrix )
	{
		TMatrix3x3( matrix.c0, matrix.c1, matrix.c2 );
	}

    template <typename T>
	FORCEINLINE constexpr TMatrix3x3<T>::TMatrix3x3( const TVector3<T>& col0, const TVector3<T>& col1, const TVector3<T>& col2 )
	{
		m00 = col0.x; m01 = col0.y; m02 = col0.z;
		m10 = col1.x; m11 = col1.y; m12 = col1.z;
		m20 = col2.x; m21 = col2.y; m22 = col2.z;
	}

    template <typename T>
	constexpr TMatrix3x3<T> TMatrix3x3<T>::Identity()
	{
		return TMatrix3x3();
	}

    template <typename T>
	inline void TMatrix3x3<T>::Transpose()
	{
		*this = TMatrix3x3( GetRow( 0 ), GetRow( 1 ), GetRow( 2 ) );
	}

    template <typename T>
	inline TMatrix3x3<T> TMatrix3x3<T>::Transposed() const
	{
		return TMatrix3x3( GetRow( 0 ), GetRow( 1 ), GetRow( 2 ) );
	}

    template <typename T>
	inline TVector3<T> TMatrix3x3<T>::GetRow( const unsigned char& i ) const
	{
		switch ( i )
		{
		case 0: return TVector3<T>( m00, m10, m20 );
		case 1: return TVector3<T>( m01, m11, m21 );
		case 2: return TVector3<T>( m02, m12, m22 );
		}

		return TVector3<T>();
	}

    template <typename T>
	inline TVector3<T> TMatrix3x3<T>::GetColumn( const unsigned char& i ) const
	{
		if ( (i > 2) ) return TVector3<T>();
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	inline TVector3<T>& TMatrix3x3<T>::operator[]( unsigned char i )
	{
		EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	inline TVector3<T> const& TMatrix3x3<T>::operator[]( unsigned char i ) const
	{
		EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE TMatrix3x3<T> TMatrix3x3<T>::operator*( const TMatrix3x3<T>& other ) const
	{
		TMatrix3x3 result = TMatrix3x3();
		const TVector3<T>& row0 = GetRow( 0 ), row1 = GetRow( 1 ), row2 = GetRow( 2 );

		result.m00 = other.c0.Dot( row0 );
		result.m10 = other.c1.Dot( row0 );
		result.m20 = other.c2.Dot( row0 );

		result.m01 = other.c0.Dot( row1 );
		result.m11 = other.c1.Dot( row1 );
		result.m21 = other.c2.Dot( row1 );

		result.m02 = other.c0.Dot( row2 );
		result.m12 = other.c1.Dot( row2 );
		result.m22 = other.c2.Dot( row2 );

		return result;
	}

    template <typename T>
	FORCEINLINE TVector3<T> TMatrix3x3<T>::operator*( const TVector3<T>& vector ) const
	{
		TVector3<T> result(
			GetRow( 0 ).Dot( vector ),
			GetRow( 1 ).Dot( vector ),
			GetRow( 2 ).Dot( vector )
		);

		return result;
	}

    template <typename T>
	inline const T* TMatrix3x3<T>::PointerToValue( void ) const
	{
		return &m00;
	}

}