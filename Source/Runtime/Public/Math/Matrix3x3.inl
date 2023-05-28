
#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3.h"

namespace EE
{
	FORCEINLINE Matrix3x3::Matrix3x3()
	{
		m0[ 0 ] = 1; m0[ 1 ] = 0; m0[ 2 ] = 0;
		m1[ 0 ] = 0; m1[ 1 ] = 1; m1[ 2 ] = 0;
		m2[ 0 ] = 0; m2[ 1 ] = 0; m2[ 2 ] = 1;
	}

	FORCEINLINE Matrix3x3::Matrix3x3( const Matrix3x3& matrix )
		: m0( matrix.m0 ), m1( matrix.m1 ), m2( matrix.m2 )
	{
	}

	FORCEINLINE Matrix3x3::Matrix3x3( const Matrix4x4& matrix )
	{
		Matrix3x3( matrix.GetRow( 0 ), matrix.GetRow( 1 ), matrix.GetRow( 2 ) );
	}

	FORCEINLINE Matrix3x3::Matrix3x3( const Vector3& row0, const Vector3& row1, const Vector3& row2 )
	{
		m0[ 0 ] = row0.x; m0[ 1 ] = row0.y; m0[ 2 ] = row0.z;
		m1[ 0 ] = row1.x; m1[ 1 ] = row1.y; m1[ 2 ] = row1.z;
		m2[ 0 ] = row2.x; m2[ 1 ] = row2.y; m2[ 2 ] = row2.z;
	}

	inline Matrix3x3 Matrix3x3::Identity()
	{
		return Matrix3x3();
	}

	inline void Matrix3x3::Transpose()
	{
		*this = Matrix3x3( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ) );
	}

	inline Matrix3x3 Matrix3x3::Transposed() const
	{
		return Matrix3x3( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ) );
	}

	inline Vector3 Matrix3x3::GetRow( const unsigned char& i ) const
	{
		if ( (i > 2) ) return Vector3();
		return ((Vector3*)this)[ i ];
	}

	inline Vector3 Matrix3x3::GetColumn( const unsigned char& i ) const
	{
		switch ( i )
		{
		case 0: return Vector3( m0[ 0 ], m1[ 0 ], m2[ 0 ] );
		case 1: return Vector3( m0[ 1 ], m1[ 1 ], m2[ 1 ] );
		case 2: return Vector3( m0[ 2 ], m1[ 2 ], m2[ 2 ] );
		}

		return Vector3();
	}

	inline Vector3& Matrix3x3::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 2, "Matrix3x3 index out of bounds" );
		return ((Vector3*)this)[ i ];
	}

	inline Vector3 const& Matrix3x3::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 2, "Matrix3x3 index out of bounds" );
		return ((Vector3*)this)[ i ];
	}

	FORCEINLINE Matrix3x3 Matrix3x3::operator*( const Matrix3x3& other ) const
	{
		Matrix3x3 result = Matrix3x3();
		const Vector3 Col0 = GetColumn( 0 ), Col1 = GetColumn( 1 ), Col2 = GetColumn( 2 );

		result.m0[ 0 ] = other.m0.Dot( Col0 );
		result.m1[ 0 ] = other.m1.Dot( Col0 );
		result.m2[ 0 ] = other.m2.Dot( Col0 );

		result.m0[ 1 ] = other.m0.Dot( Col1 );
		result.m1[ 1 ] = other.m1.Dot( Col1 );
		result.m2[ 1 ] = other.m2.Dot( Col1 );

		result.m0[ 2 ] = other.m0.Dot( Col2 );
		result.m1[ 2 ] = other.m1.Dot( Col2 );
		result.m2[ 2 ] = other.m2.Dot( Col2 );

		return result;
	}

	FORCEINLINE Vector3 Matrix3x3::operator*( const Vector3& vector ) const
	{
		Vector3 result(
			GetColumn( 0 ).Dot( vector ),
			GetColumn( 1 ).Dot( vector ),
			GetColumn( 2 ).Dot( vector )
		);

		return result;
	}

	inline const float* Matrix3x3::PointerToValue( void ) const
	{
		return &m0[ 0 ];
	}

}