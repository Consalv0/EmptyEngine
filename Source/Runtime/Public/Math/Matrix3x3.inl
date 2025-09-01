
namespace EE::Math
{
    template <typename T>
    FORCEINLINE constexpr TMatrix3x3<T>::TMatrix3x3()
    {
        c0r0 = 1; c0r1 = 0; c0r2 = 0;
        c1r0 = 0; c1r1 = 1; c1r2 = 0;
        c2r0 = 0; c2r1 = 0; c2r2 = 1;
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
        c0r0 = col0.x; c0r1 = col0.y; c0r2 = col0.z;
        c1r0 = col1.x; c1r1 = col1.y; c1r2 = col1.z;
        c2r0 = col2.x; c2r1 = col2.y; c2r2 = col2.z;
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
        EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
        switch ( i )
        {
        case 0: return TVector3<T>( c0r0, c1r0, c2r0 );
        case 1: return TVector3<T>( c0r1, c1r1, c2r1 );
        case 2: return TVector3<T>( c0r2, c1r2, c2r2 );
        }
    }

    template <typename T>
    inline TVector3<T> TMatrix3x3<T>::GetColumn( const unsigned char& i ) const
    {
        EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
        return this[ i ];
    }

    template <typename T>
    inline TVector3<T>& TMatrix3x3<T>::operator[]( unsigned char i )
    {
        EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
        return this[ i ];
    }

    template <typename T>
    inline TVector3<T> const& TMatrix3x3<T>::operator[]( unsigned char i ) const
    {
        EE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
        return this[ i ];
    }

    template <typename T>
    FORCEINLINE TMatrix3x3<T> TMatrix3x3<T>::operator*( const TMatrix3x3<T>& other ) const
    {
        TMatrix3x3 result = TMatrix3x3();
        const TVector3<T>& row0 = GetRow( 0 ), row1 = GetRow( 1 ), row2 = GetRow( 2 );

        result.c0r0 = other.c0.Dot( row0 );
        result.c1r0 = other.c1.Dot( row0 );
        result.c2r0 = other.c2.Dot( row0 );

        result.c0r1 = other.c0.Dot( row1 );
        result.c1r1 = other.c1.Dot( row1 );
        result.c2r1 = other.c2.Dot( row1 );

        result.c0r2 = other.c0.Dot( row2 );
        result.c1r2 = other.c1.Dot( row2 );
        result.c2r2 = other.c2.Dot( row2 );

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
    inline const T* TMatrix3x3<T>::PointerToValue() const
    {
        return &c0r0;
    }

}