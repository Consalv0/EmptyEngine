
#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

namespace EE
{
	FORCEINLINE Matrix4x4::Matrix4x4()
	{
		m0[ 0 ] = 1; m0[ 1 ] = 0; m0[ 2 ] = 0; m0[ 3 ] = 0;
		m1[ 0 ] = 0; m1[ 1 ] = 1; m1[ 2 ] = 0; m1[ 3 ] = 0;
		m2[ 0 ] = 0; m2[ 1 ] = 0; m2[ 2 ] = 1; m2[ 3 ] = 0;
		m3[ 0 ] = 0; m3[ 1 ] = 0; m3[ 2 ] = 0; m3[ 3 ] = 1;
	}

	FORCEINLINE Matrix4x4::Matrix4x4( const Matrix4x4& other )
		: m0( other.m0 ), m1( other.m1 ), m2( other.m2 ), m3( other.m3 )
	{
	}

	FORCEINLINE Matrix4x4::Matrix4x4( const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4 row3 )
	{
		m0[ 0 ] = row0.x; m0[ 1 ] = row0.y; m0[ 2 ] = row0.z; m0[ 3 ] = row0.w;
		m1[ 0 ] = row1.x; m1[ 1 ] = row1.y; m1[ 2 ] = row1.z; m1[ 3 ] = row1.w;
		m2[ 0 ] = row2.x; m2[ 1 ] = row2.y; m2[ 2 ] = row2.z; m2[ 3 ] = row2.w;
		m3[ 0 ] = row3.x; m3[ 1 ] = row3.y; m3[ 2 ] = row3.z; m3[ 3 ] = row3.w;
	}

	inline Matrix4x4::Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33 )
	{

		m0[ 0 ] = m00; m0[ 1 ] = m01; m0[ 2 ] = m02; m0[ 3 ] = m03;
		m1[ 0 ] = m10; m1[ 1 ] = m11; m1[ 2 ] = m12; m1[ 3 ] = m13;
		m2[ 0 ] = m20; m2[ 1 ] = m21; m2[ 2 ] = m22; m2[ 3 ] = m23;
		m3[ 0 ] = m30; m3[ 1 ] = m31; m3[ 2 ] = m32; m3[ 3 ] = m33;
	}

	inline Matrix4x4 Matrix4x4::Identity()
	{
		return Matrix4x4();
	}

	inline Matrix4x4 Matrix4x4::Perspective( const float& fov, const float& aspect, const float& near, const float& far )
	{
		float const tanHalfFOV = tan( fov / 2.F );
		Matrix4x4 result(
			1.F / (aspect * tanHalfFOV), 0.F, 0.F, 0.F,
			0.F, 1.F / (tanHalfFOV), 0.F, 0.F,
			0.F, 0.F, -(far + near) / (far - near), -1.F,
			0.F, 0.F, -(2.F * far * near) / (far - near), 0.F
		);
		return result;
	}

	inline Matrix4x4 Matrix4x4::Orthographic( const float& left, const float& right, const float& bottom, const float& top )
	{
		Matrix4x4 result(
			2.F / (right - left), 0.F, 0.F, 0.F,
			0.F, 2.F / (top - bottom), 0.F, 0.F,
			0.F, 0.F, 1.F, 0.F,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.F, 1.F
		);
		return result;
	}

	inline Matrix4x4 Matrix4x4::Orthographic( const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far )
	{
		Matrix4x4 result(
			2.F / (right - left), 0.F, 0.F, 0.F,
			0.F, 2.F / (top - bottom), 0.F, 0.F,
			0.F, 0.F, 1.F / (near - far), 0.F,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), near / (near - far), 1.F
		);
		return result;
	}

	inline Matrix4x4 Matrix4x4::LookAt( const Vector3& eye, const Vector3& target, const Vector3& up )
	{
		Vector3 const forward( (eye - target).Normalized() );
		Vector3 const side( forward.Cross( up ).Normalized() );
		Vector3 const upper( side.Cross( forward ) );

		return Matrix4x4(
			side.x, side.y, side.z, 0.F,
			upper.x, upper.y, upper.z, 0.F,
			-forward.x, -forward.y, -forward.z, 0.F,
			eye.x, eye.y, eye.z, 1.F
		);
	}

	inline Matrix4x4 Matrix4x4::Translation( const Vector3& vector )
	{
		return Matrix4x4(
			1.F, 0.F, 0.F, 0.F,
			0.F, 1.F, 0.F, 0.F,
			0.F, 0.F, 1.F, 0.F,
			vector.x, vector.y, vector.z, 1.F
		);
	}

	inline Matrix4x4 Matrix4x4::Scaling( const Vector3& vector )
	{
		return Matrix4x4(
			vector.x, 0.F, 0.F, 0.F,
			0.F, vector.y, 0.F, 0.F,
			0.F, 0.F, vector.z, 0.F,
			0.F, 0.F, 0.F, 1.F
		);
	}

	inline Matrix4x4 Matrix4x4::Rotation( const Vector3& axis, const float& radians )
	{
		float const cosA = cos( radians );
		float const sinA = sin( radians );

		Vector3 axisN( axis.Normalized() );
		Vector3 temp( axisN * (1.F - cosA) );

		Matrix4x4 rotation;
		rotation.m0[ 0 ] = cosA + temp[ 0 ] * axisN[ 0 ];
		rotation.m0[ 1 ] = temp[ 0 ] * axisN[ 1 ] + sinA * axisN[ 2 ];
		rotation.m0[ 2 ] = temp[ 0 ] * axisN[ 2 ] - sinA * axisN[ 1 ];

		rotation.m1[ 0 ] = temp[ 1 ] * axisN[ 0 ] - sinA * axisN[ 2 ];
		rotation.m1[ 1 ] = cosA + temp[ 1 ] * axisN[ 1 ];
		rotation.m1[ 2 ] = temp[ 1 ] * axisN[ 2 ] + sinA * axisN[ 0 ];

		rotation.m2[ 0 ] = temp[ 2 ] * axisN[ 0 ] + sinA * axisN[ 1 ];
		rotation.m2[ 1 ] = temp[ 2 ] * axisN[ 1 ] - sinA * axisN[ 0 ];
		rotation.m2[ 2 ] = cosA + temp[ 2 ] * axisN[ 2 ];

		return rotation;
	}

	inline Matrix4x4 Matrix4x4::Rotation( const Vector3& euler )
	{
		Matrix4x4 result;
		float sinR, sinP, sinY, cosR, cosP, cosY;

		sinY = std::sin( euler[ Yaw ] * Math::DegToRad );
		cosY = std::cos( euler[ Yaw ] * Math::DegToRad );
		sinP = std::sin( euler[ Pitch ] * Math::DegToRad );
		cosP = std::cos( euler[ Pitch ] * Math::DegToRad );
		sinR = std::sin( euler[ Roll ] * Math::DegToRad );
		cosR = std::cos( euler[ Roll ] * Math::DegToRad );

		result.m0[ 0 ] = cosP * cosY;
		result.m0[ 1 ] = cosP * sinY;
		result.m0[ 2 ] = -sinP;

		result.m1[ 0 ] = sinR * sinP * cosY + cosR * -sinY;
		result.m1[ 1 ] = sinR * sinP * sinY + cosR * cosY;
		result.m1[ 2 ] = sinR * cosP;

		result.m2[ 0 ] = (cosR * sinP * cosY + -sinR * -sinY);
		result.m2[ 1 ] = (cosR * sinP * sinY + -sinR * cosY);
		result.m2[ 2 ] = cosR * cosP;

		result.m3[ 0 ] = 0.F;
		result.m3[ 1 ] = 0.F;
		result.m3[ 2 ] = 0.F;
		result.m3[ 3 ] = 1.F;

		return result;
	}

	inline Matrix4x4 Matrix4x4::Rotation( const Quaternion& quat )
	{
		return quat.ToMatrix4x4();
	}

	inline void Matrix4x4::Transpose()
	{
		Matrix4x4 result = Matrix4x4( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ), GetColumn( 3 ) );
		*this = result;
	}

	inline Matrix4x4 Matrix4x4::Transposed() const
	{
		return Matrix4x4( GetColumn( 0 ), GetColumn( 1 ), GetColumn( 2 ), GetColumn( 3 ) );
	}

	inline Matrix4x4 Matrix4x4::Inversed() const
	{
		float coef00 = m2[ 2 ] * m3[ 3 ] - m3[ 2 ] * m2[ 3 ];
		float coef02 = m1[ 2 ] * m3[ 3 ] - m3[ 2 ] * m1[ 3 ];
		float coef03 = m1[ 2 ] * m2[ 3 ] - m2[ 2 ] * m1[ 3 ];

		float coef04 = m2[ 1 ] * m3[ 3 ] - m3[ 1 ] * m2[ 3 ];
		float coef06 = m1[ 1 ] * m3[ 3 ] - m3[ 1 ] * m1[ 3 ];
		float coef07 = m1[ 1 ] * m2[ 3 ] - m2[ 1 ] * m1[ 3 ];

		float coef08 = m2[ 1 ] * m3[ 2 ] - m3[ 1 ] * m2[ 2 ];
		float coef10 = m1[ 1 ] * m3[ 2 ] - m3[ 1 ] * m1[ 2 ];
		float coef11 = m1[ 1 ] * m2[ 2 ] - m2[ 1 ] * m1[ 2 ];

		float coef12 = m2[ 0 ] * m3[ 3 ] - m3[ 0 ] * m2[ 3 ];
		float coef14 = m1[ 0 ] * m3[ 3 ] - m3[ 0 ] * m1[ 3 ];
		float coef15 = m1[ 0 ] * m2[ 3 ] - m2[ 0 ] * m1[ 3 ];

		float coef16 = m2[ 0 ] * m3[ 2 ] - m3[ 0 ] * m2[ 2 ];
		float coef18 = m1[ 0 ] * m3[ 2 ] - m3[ 0 ] * m1[ 2 ];
		float coef19 = m1[ 0 ] * m2[ 2 ] - m2[ 0 ] * m1[ 2 ];

		float coef20 = m2[ 0 ] * m3[ 1 ] - m3[ 0 ] * m2[ 1 ];
		float coef22 = m1[ 0 ] * m3[ 1 ] - m3[ 0 ] * m1[ 1 ];
		float coef23 = m1[ 0 ] * m2[ 1 ] - m2[ 0 ] * m1[ 1 ];

		Vector4 fac0( coef00, coef00, coef02, coef03 );
		Vector4 fac1( coef04, coef04, coef06, coef07 );
		Vector4 fac2( coef08, coef08, coef10, coef11 );
		Vector4 fac3( coef12, coef12, coef14, coef15 );
		Vector4 fac4( coef16, coef16, coef18, coef19 );
		Vector4 fac5( coef20, coef20, coef22, coef23 );

		Vector4 vec0( m1[ 0 ], m0[ 0 ], m0[ 0 ], m0[ 0 ] );
		Vector4 vec1( m1[ 1 ], m0[ 1 ], m0[ 1 ], m0[ 1 ] );
		Vector4 vec2( m1[ 2 ], m0[ 2 ], m0[ 2 ], m0[ 2 ] );
		Vector4 vec3( m1[ 3 ], m0[ 3 ], m0[ 3 ], m0[ 3 ] );

		Vector4 inv0( vec1 * fac0 - vec2 * fac1 + vec3 * fac2 );
		Vector4 inv1( vec0 * fac0 - vec2 * fac3 + vec3 * fac4 );
		Vector4 inv2( vec0 * fac1 - vec1 * fac3 + vec3 * fac5 );
		Vector4 inv3( vec0 * fac2 - vec1 * fac4 + vec2 * fac5 );

		Vector4 signA( +1.F, -1.F, +1.F, -1.F );
		Vector4 signB( -1.F, +1.F, -1.F, +1.F );

		Matrix4x4 result( inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB );

		// Vector4 row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

		Vector4 dot0( GetRow( 0 ) * result.GetColumn( 0 ) );
		float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

		float oneOverDeterminant = 1.F / dot1;

		return result * oneOverDeterminant;
	}

	inline Vector4 Matrix4x4::GetRow( const unsigned char& i ) const
	{
		if ( i > 3 ) return Vector4();
		return ((Vector4*)this)[ i ];
	}

	inline Vector4 Matrix4x4::GetColumn( const unsigned char& i ) const
	{
		switch ( i )
		{
		case 0: return { m0[ 0 ], m1[ 0 ], m2[ 0 ], m3[ 0 ] };
		case 1: return { m0[ 1 ], m1[ 1 ], m2[ 1 ], m3[ 1 ] };
		case 2: return { m0[ 2 ], m1[ 2 ], m2[ 2 ], m3[ 2 ] };
		case 3: return { m0[ 3 ], m1[ 3 ], m2[ 3 ], m3[ 3 ] };
		}

		return Vector4();
	}

	inline HOST_DEVICE Vector3 Matrix4x4::ExtractTranslation() const
	{
		return GetRow( 3 );
	}

	inline HOST_DEVICE Quaternion Matrix4x4::ExtractRotation() const
	{
		return Quaternion::FromLookRotation( GetRow( 2 ), GetRow( 1 ) );
	}

	inline HOST_DEVICE Vector3 Matrix4x4::ExtractScale() const
	{
		Vector3 scale(
			GetRow( 0 ).Magnitude(),
			GetRow( 1 ).Magnitude(),
			GetRow( 2 ).Magnitude()
		);
		return scale;
	}

	inline Vector4& Matrix4x4::operator[]( unsigned char i )
	{
		EE_CORE_ASSERT( i <= 3, "Matrix4x4 index out of bounds" );
		return ((Vector4*)this)[ i ];
	}

	inline Vector4 const& Matrix4x4::operator[]( unsigned char i ) const
	{
		EE_CORE_ASSERT( i <= 3, "Matrix4x4 index out of bounds" );
		return ((Vector4*)this)[ i ];
	}

	inline HOST_DEVICE Vector4 Matrix4x4::MultiplyPoint( const Vector3& vector ) const
	{
		return *this * Vector4( vector, 1.F );
	}

	inline Vector3 Matrix4x4::MultiplyVector( const Vector3& vector ) const
	{
		return *this * vector;
	}

	FORCEINLINE Matrix4x4 Matrix4x4::operator*( const Matrix4x4& other ) const
	{
		Matrix4x4 result = Matrix4x4();

		const Vector4 col0 = GetColumn( 0 ), col1 = GetColumn( 1 ), col2 = GetColumn( 2 ), col3 = GetColumn( 3 );

		result.m0[ 0 ] = other.m0.Dot( col0 );
		result.m1[ 0 ] = other.m1.Dot( col0 );
		result.m2[ 0 ] = other.m2.Dot( col0 );
		result.m3[ 0 ] = other.m3.Dot( col0 );

		result.m0[ 1 ] = other.m0.Dot( col1 );
		result.m1[ 1 ] = other.m1.Dot( col1 );
		result.m2[ 1 ] = other.m2.Dot( col1 );
		result.m3[ 1 ] = other.m3.Dot( col1 );

		result.m0[ 2 ] = other.m0.Dot( col2 );
		result.m1[ 2 ] = other.m1.Dot( col2 );
		result.m2[ 2 ] = other.m2.Dot( col2 );
		result.m3[ 2 ] = other.m3.Dot( col2 );

		result.m0[ 3 ] = other.m0.Dot( col3 );
		result.m1[ 3 ] = other.m1.Dot( col3 );
		result.m2[ 3 ] = other.m2.Dot( col3 );
		result.m3[ 3 ] = other.m3.Dot( col3 );

		return result;
	}

	FORCEINLINE Vector4 Matrix4x4::operator*( const Vector4& vector ) const
	{
		Vector4 result(
			GetColumn( 0 ).Dot( vector ),
			GetColumn( 1 ).Dot( vector ),
			GetColumn( 2 ).Dot( vector ),
			GetColumn( 3 ).Dot( vector )
		);

		return result;
	}

	FORCEINLINE Vector3 Matrix4x4::operator*( const Vector3& vector ) const
	{
		Vector4 const vect = Vector4( vector, 0.F );
		Vector3 result(
			GetColumn( 0 ).Dot( vect ),
			GetColumn( 1 ).Dot( vect ),
			GetColumn( 2 ).Dot( vect )
		);
		return result;
	}

	FORCEINLINE Matrix4x4 Matrix4x4::operator*( const float& value ) const
	{
		Matrix4x4 result( *this );

		result.m0 *= value;
		result.m1 *= value;
		result.m2 *= value;
		result.m3 *= value;

		return result;
	}

	FORCEINLINE Matrix4x4 Matrix4x4::operator/( const float& value ) const
	{
		Matrix4x4 result( *this );

		result.m0 /= value;
		result.m1 /= value;
		result.m2 /= value;
		result.m3 /= value;

		return result;
	}

	inline const float* Matrix4x4::PointerToValue( void ) const
	{
		return &m0[ 0 ];
	}
}
