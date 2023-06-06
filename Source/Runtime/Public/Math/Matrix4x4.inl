
#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

namespace EE
{
	FORCEINLINE Matrix4x4::Matrix4x4() :
		m00(1), m01(0), m02(0), m03(0),
		m10(0), m11(1), m12(0), m13(0),
		m20(0), m21(0), m22(1), m23(0),
		m30(0), m31(0), m32(0), m33(1)
	{
	}

	FORCEINLINE Matrix4x4::Matrix4x4( const Matrix4x4& other )
		: m0( other.m0 ), m1( other.m1 ), m2( other.m2 ), m3( other.m3 )
	{
	}

	FORCEINLINE Matrix4x4::Matrix4x4( const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4 row3 )
	{
		m00 = row0.x; m01 = row0.y; m02 = row0.z; m03 = row0.w;
		m10 = row1.x; m11 = row1.y; m12 = row1.z; m13 = row1.w;
		m20 = row2.x; m21 = row2.y; m22 = row2.z; m23 = row2.w;
		m30 = row3.x; m31 = row3.y; m32 = row3.z; m33 = row3.w;
	}

	inline Matrix4x4::Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33 ) :
		m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33)
	{
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
		rotation.m00 = cosA + temp.x * axisN.x;
		rotation.m01 = temp.x * axisN.y + sinA * axisN.z;
		rotation.m02 = temp.x * axisN.z - sinA * axisN.y;

		rotation.m10 = temp.y * axisN.x - sinA * axisN.z;
		rotation.m11 = cosA + temp.y * axisN.y;
		rotation.m12 = temp.y * axisN.z + sinA * axisN.x;

		rotation.m20 = temp.z * axisN.x + sinA * axisN.y;
		rotation.m21 = temp.z * axisN.y - sinA * axisN.x;
		rotation.m22 = cosA + temp.z * axisN.z;

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

		result.m00 = cosP * cosY;
		result.m01 = cosP * sinY;
		result.m02 = -sinP;

		result.m10 = sinR * sinP * cosY + cosR * -sinY;
		result.m11 = sinR * sinP * sinY + cosR * cosY;
		result.m12 = sinR * cosP;

		result.m20 = (cosR * sinP * cosY + -sinR * -sinY);
		result.m21 = (cosR * sinP * sinY + -sinR * cosY);
		result.m22 = cosR * cosP;

		result.m30 = 0.F;
		result.m31 = 0.F;
		result.m32 = 0.F;
		result.m33 = 1.F;

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
		float coef00 = m22 * m33 - m32 * m23;
		float coef02 = m12 * m33 - m32 * m13;
		float coef03 = m12 * m23 - m22 * m13;

		float coef04 = m21 * m33 - m31 * m23;
		float coef06 = m11 * m33 - m31 * m13;
		float coef07 = m11 * m23 - m21 * m13;

		float coef08 = m21 * m32 - m31 * m22;
		float coef10 = m11 * m32 - m31 * m12;
		float coef11 = m11 * m22 - m21 * m12;

		float coef12 = m20 * m33 - m30 * m23;
		float coef14 = m10 * m33 - m30 * m13;
		float coef15 = m10 * m23 - m20 * m13;

		float coef16 = m20 * m32 - m30 * m22;
		float coef18 = m10 * m32 - m30 * m12;
		float coef19 = m10 * m22 - m20 * m12;

		float coef20 = m20 * m31 - m30 * m21;
		float coef22 = m10 * m31 - m30 * m11;
		float coef23 = m10 * m21 - m20 * m11;

		Vector4 fac0( coef00, coef00, coef02, coef03 );
		Vector4 fac1( coef04, coef04, coef06, coef07 );
		Vector4 fac2( coef08, coef08, coef10, coef11 );
		Vector4 fac3( coef12, coef12, coef14, coef15 );
		Vector4 fac4( coef16, coef16, coef18, coef19 );
		Vector4 fac5( coef20, coef20, coef22, coef23 );

		Vector4 vec0( m10, m00, m00, m00 );
		Vector4 vec1( m11, m01, m01, m01 );
		Vector4 vec2( m12, m02, m02, m02 );
		Vector4 vec3( m13, m03, m03, m03 );

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
		case 0: return { m00, m10, m20, m30 };
		case 1: return { m01, m11, m21, m31 };
		case 2: return { m02, m12, m22, m32 };
		case 3: return { m03, m13, m23, m33 };
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

		result.m00 = other.m0.Dot( col0 );
		result.m10 = other.m1.Dot( col0 );
		result.m20 = other.m2.Dot( col0 );
		result.m30 = other.m3.Dot( col0 );

		result.m01 = other.m0.Dot( col1 );
		result.m11 = other.m1.Dot( col1 );
		result.m21 = other.m2.Dot( col1 );
		result.m31 = other.m3.Dot( col1 );

		result.m02 = other.m0.Dot( col2 );
		result.m12 = other.m1.Dot( col2 );
		result.m22 = other.m2.Dot( col2 );
		result.m32 = other.m3.Dot( col2 );

		result.m03 = other.m0.Dot( col3 );
		result.m13 = other.m1.Dot( col3 );
		result.m23 = other.m2.Dot( col3 );
		result.m33 = other.m3.Dot( col3 );

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
		return &m00;
	}
}
