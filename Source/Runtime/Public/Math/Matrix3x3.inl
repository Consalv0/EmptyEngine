
#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE TMatrix3x3<T>::TMatrix3x3()
	{
		m00 = 1; m01 = 0; m02 = 0;
		m10 = 0; m11 = 1; m12 = 0;
		m20 = 0; m21 = 0; m22 = 1;
	}

    template <typename T>
	FORCEINLINE TMatrix3x3<T>::TMatrix3x3( const TMatrix3x3& matrix )
		: m0( matrix.m0 ), m1( matrix.m1 ), m2( matrix.m2 )
	{
	}

    template <typename T>
	FORCEINLINE TMatrix3x3<T>::TMatrix3x3( const TMatrix4x4<T>& matrix )
	{
		TMatrix3x3( matrix.GetRow( 0 ), matrix.GetRow( 1 ), matrix.GetRow( 2 ) );
	}

    template <typename T>
	FORCEINLINE TMatrix3x3<T>::TMatrix3x3( const TVector3<T>& row0, const TVector3<T>& row1, const TVector3<T>& row2 )
	{
		m00 = row0.x; m01 = row0.y; m02 = row0.z;
		m10 = row1.x; m11 = row1.y; m12 = row1.z;
		m20 = row2.x; m21 = row2.y; m22 = row2.z;
	}

    template <typename T>
	inline TMatrix3x3<T> TMatrix3x3<T>::Identity()
	{
		return TMatrix3x3();
	}

    template <typename T>
	inline void TMatrix3x3<T>::Transpose()
	{
		*this = TMatrix3x3( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ) );
	}

    template <typename T>
	inline TMatrix3x3<T> TMatrix3x3<T>::Transposed() const
	{
		return TMatrix3x3( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ) );
	}

    template <typename T>
	inline TVector3<T> TMatrix3x3<T>::GetRow( const unsigned char& i ) const
	{
		if ( (i > 2) ) return TVector3<T>();
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	inline TVector3<T> TMatrix3x3<T>::GetColumn( const unsigned char& i ) const
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
	inline TVector3<T>& TMatrix3x3<T>::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	inline TVector3<T> const& TMatrix3x3<T>::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "TMatrix3x3 index out of bounds" );
		return ((TVector3<T>*)this)[ i ];
	}

    template <typename T>
	FORCEINLINE TMatrix3x3<T> TMatrix3x3<T>::operator*( const TMatrix3x3<T>& other ) const
	{
		TMatrix3x3 result = TMatrix3x3();
		const TVector3<T> Col0 = GetColumn( 0 ), Col1 = GetColumn( 1 ), Col2 = GetColumn( 2 );

		result.m00 = other.m0.Dot( Col0 );
		result.m10 = other.m1.Dot( Col0 );
		result.m20 = other.m2.Dot( Col0 );

		result.m01 = other.m0.Dot( Col1 );
		result.m11 = other.m1.Dot( Col1 );
		result.m21 = other.m2.Dot( Col1 );

		result.m02 = other.m0.Dot( Col2 );
		result.m12 = other.m1.Dot( Col2 );
		result.m22 = other.m2.Dot( Col2 );

		return result;
	}

    template <typename T>
	FORCEINLINE TVector3<T> TMatrix3x3<T>::operator*( const TVector3<T>& vector ) const
	{
		TVector3<T> result(
			GetColumn( 0 ).Dot( vector ),
			GetColumn( 1 ).Dot( vector ),
			GetColumn( 2 ).Dot( vector )
		);

		return result;
	}

    template <typename T>
	inline const T* TMatrix3x3<T>::PointerToValue( void ) const
	{
		return &m0[ 0 ];
	}

}