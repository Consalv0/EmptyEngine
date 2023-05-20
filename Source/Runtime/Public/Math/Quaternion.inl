#pragma once

#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3.h"
#include "Math/Quaternion.h"

namespace EEngine
{
	FORCEINLINE Quaternion::Quaternion()
		:w( 1 ), x( 0 ), y( 0 ), z( 0 )
	{
	}

	FORCEINLINE Quaternion::Quaternion( Quaternion const& other )
		: w( other.w ), x( other.x ), y( other.y ), z( other.z )
	{
	}

	FORCEINLINE Quaternion::Quaternion( float const& Scale, Vector3 const& vector )
		: w( Scale ), x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

	FORCEINLINE Quaternion::Quaternion( float const& w, float const& x, float const& y, float const& z )
		: w( w ), x( x ), y( y ), z( z )
	{
	}

	inline Quaternion Quaternion::FromEulerAngles( Vector3 const& EulerAngles )
	{
		const float Scale = Math::DegToRad * 0.5F;
		float HalfRoll = EulerAngles[ Roll ] * Scale;
		float HalfPitch = EulerAngles[ Pitch ] * Scale;
		float HalfYaw = EulerAngles[ Yaw ] * Scale;

		float SR = std::sin( HalfRoll );
		float CR = std::cos( HalfRoll );
		float SP = std::sin( HalfPitch );
		float CP = std::cos( HalfPitch );
		float SY = std::sin( HalfYaw );
		float CY = std::cos( HalfYaw );

		Quaternion EulerToQuat;
		EulerToQuat.x = (CY * SP * CR) + (SY * CP * SR);
		EulerToQuat.y = (SY * CP * CR) - (CY * SP * SR);
		EulerToQuat.z = (CY * CP * SR) - (SY * SP * CR);
		EulerToQuat.w = (CY * CP * CR) + (SY * SP * SR);
		return EulerToQuat;
	}

	FORCEINLINE Quaternion Quaternion::FromToRotation( Vector3 const& From, Vector3 const& To )
	{
		Vector3 Half = From + To;
		Half.Normalize();

		return Quaternion(
			From.Dot( Half ),
			From.y * Half.z - From.z * Half.y,
			From.z * Half.x - From.x * Half.z,
			From.x * Half.y - From.y * Half.x
		).Normalized();
	}

	inline Quaternion Quaternion::FromAxisAngle( Vector3 const& Axis, float const& Radians )
	{
		float Sine = sinf( Radians * .5F );

		return Quaternion(
			cosf( Radians * .5F ),
			Axis.x * Sine,
			Axis.y * Sine,
			Axis.z * Sine
		);
	}

	inline Quaternion Quaternion::FromLookRotation( Vector3 const& Forward, Vector3 const& Up )
	{
		const Vector3 Normal = Forward.Normalized();
		const Vector3 Tangent = Vector3::Cross( Up == Normal ? Up + 0.001F : Up, Normal ).Normalized();
		const Vector3 Bitangent = Vector3::Cross( Normal, Tangent );

		Matrix3x3 LookSpace(
			Tangent, Bitangent, Normal
		);

		return Quaternion::FromMatrix( LookSpace );
	}

	inline Quaternion Quaternion::FromMatrix( Matrix3x3 const& matrix )
	{
		const float tr = matrix.m0[ 0 ] + matrix.m1[ 1 ] + matrix.m2[ 2 ];

		if ( tr > 0.F )
		{
			float num0 = sqrtf( tr + 1.F );
			float num1 = 0.5f / num0;
			return Quaternion(
				num0 * 0.5f,
				(matrix.m1[ 2 ] - matrix.m2[ 1 ]) * num1,
				(matrix.m2[ 0 ] - matrix.m0[ 2 ]) * num1,
				(matrix.m0[ 1 ] - matrix.m1[ 0 ]) * num1
			);
		}
		if ( (matrix.m0[ 0 ] >= matrix.m1[ 1 ]) && (matrix.m0[ 0 ] >= matrix.m2[ 2 ]) )
		{
			float num7 = sqrtf( ((1.F + matrix.m0[ 0 ]) - matrix.m1[ 1 ]) - matrix.m2[ 2 ] );
			float num4 = 0.5f / num7;
			return Quaternion(
				(matrix.m1[ 2 ] - matrix.m2[ 1 ]) * num4,
				0.5f * num7,
				(matrix.m0[ 1 ] + matrix.m1[ 0 ]) * num4,
				(matrix.m0[ 2 ] + matrix.m2[ 0 ]) * num4
			);
		}
		if ( matrix.m1[ 1 ] > matrix.m2[ 2 ] )
		{
			float num6 = sqrtf( ((1.F + matrix.m1[ 1 ]) - matrix.m0[ 0 ]) - matrix.m2[ 2 ] );
			float num3 = 0.5f / num6;
			return Quaternion(
				(matrix.m2[ 0 ] - matrix.m0[ 2 ]) * num3,
				(matrix.m1[ 0 ] + matrix.m0[ 1 ]) * num3,
				0.5f * num6,
				(matrix.m2[ 1 ] + matrix.m1[ 2 ]) * num3
			);
		}

		float num5 = sqrtf( ((1.F + matrix.m2[ 2 ]) - matrix.m0[ 0 ]) - matrix.m1[ 1 ] );
		float num2 = 0.5F / num5;
		return Quaternion(
			(matrix.m0[ 1 ] - matrix.m1[ 0 ]) * num2,
			(matrix.m2[ 0 ] + matrix.m0[ 2 ]) * num2,
			(matrix.m2[ 1 ] + matrix.m1[ 2 ]) * num2,
			0.5F * num5
		);
	}

	inline void Quaternion::Interpolate( Quaternion& Out, const Quaternion& start, const Quaternion& end, float Factor )
	{
		float CosTheta = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

		// Adjust signs (if necessary)
		Quaternion AdjEnd = end;
		if ( CosTheta < 0.F )
		{
			CosTheta = -CosTheta;
			AdjEnd.x = -AdjEnd.x;   // Reverse all signs
			AdjEnd.y = -AdjEnd.y;
			AdjEnd.z = -AdjEnd.z;
			AdjEnd.w = -AdjEnd.w;
		}

		// Calculate coefficients
		float sclp, sclq;
		if ( (1.F - CosTheta) > 0.0001F )
		{
			// Standard case (slerp)
			float omega = std::acos( CosTheta ); // extract theta from dot product's cos theta
			float sinom = std::sin( omega );
			sclp = std::sin( (1.F - Factor) * omega ) / sinom;
			sclq = std::sin( Factor * omega ) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = 1.F - Factor;
			sclq = Factor;
		}

		Out.x = sclp * start.x + sclq * AdjEnd.x;
		Out.y = sclp * start.y + sclq * AdjEnd.y;
		Out.z = sclp * start.z + sclq * AdjEnd.z;
		Out.w = sclp * start.w + sclq * AdjEnd.w;
	}

	inline float Quaternion::Magnitude() const
	{
		return sqrtf( x * x + y * y + z * z + w * w );
	}

	inline float Quaternion::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline void Quaternion::Normalize()
	{
		if ( MagnitudeSquared() == 0 )
		{
			w = 1; x = 0; y = 0; z = 0;
		}
		else
		{
			*this /= Magnitude();
		}
	}

	inline Quaternion Quaternion::Normalized() const
	{
		if ( MagnitudeSquared() == 0 ) return Quaternion();
		Quaternion Result = Quaternion( *this );
		return Result /= Magnitude();
	}

	inline Quaternion Quaternion::Conjugated() const
	{
		return Quaternion( GetScalar(), GetVector() * -1.F );
	}

	inline Quaternion Quaternion::Inversed() const
	{
		float AbsoluteValue = Magnitude();
		AbsoluteValue *= AbsoluteValue;
		AbsoluteValue = 1 / AbsoluteValue;

		Quaternion ConjugateVal = Conjugated();

		float Scalar = ConjugateVal.GetScalar() * AbsoluteValue;
		Vector3 Imaginary = ConjugateVal.GetVector() * AbsoluteValue;

		return Quaternion( Scalar, Imaginary );
	}

	inline Matrix4x4 Quaternion::ToMatrix4x4() const
	{
		Matrix4x4 Result;
		float xx( x * x );
		float yy( y * y );
		float zz( z * z );
		float xz( x * z );
		float xy( x * y );
		float yz( y * z );
		float wx( w * x );
		float wy( w * y );
		float wz( w * z );

		Result[ 0 ][ 0 ] = 1.F - 2.F * (yy + zz);
		Result[ 0 ][ 1 ] = 2.F * (xy + wz);
		Result[ 0 ][ 2 ] = 2.F * (xz - wy);

		Result[ 1 ][ 0 ] = 2.F * (xy - wz);
		Result[ 1 ][ 1 ] = 1.F - 2.f * (xx + zz);
		Result[ 1 ][ 2 ] = 2.F * (yz + wx);

		Result[ 2 ][ 0 ] = 2.F * (xz + wy);
		Result[ 2 ][ 1 ] = 2.F * (yz - wx);
		Result[ 2 ][ 2 ] = 1.F - 2.F * (xx + yy);
		return Result;
	}

	inline float Quaternion::GetPitch() const
	{
		float Pitch;
		const float SingularityTest = x * y + z * w;
		if ( Math::Abs( SingularityTest ) > 0.499995F )
		{
			return 0.F;
		}
		else
		{
			Pitch = Math::Atan2( (2.F * x * w) - (2.F * y * z), 1.F - (2.F * Math::Square( x )) - (2.F * Math::Square( z )) );
		}
		return Pitch * Math::RadToDegree;
	}

	inline float Quaternion::GetYaw() const
	{
		float Yaw;
		const float SingularityTest = x * y + z * w;
		if ( SingularityTest > 0.499995F )
		{
			Yaw = 2.F * Math::Atan2( x, w );
		}
		else if ( SingularityTest < -0.49999F )
		{
			Yaw = -2.F * Math::Atan2( x, w );
		}
		else
		{
			const float sqy = y * y;
			const float sqz = z * z;
			Yaw = Math::Atan2( (2.F * y * w) - (2.F * x * z), 1.F - (2.F * sqy) - (2.F * sqz) );
		}
		return Yaw * Math::RadToDegree;
	}

	inline float Quaternion::GetRoll() const
	{
		float Roll;
		const float SingularityTest = x * y + z * w;
		if ( SingularityTest > 0.499995F )
		{
			Roll = Math::HalfPi;
		}
		else if ( SingularityTest < -0.499995F )
		{
			Roll = -Math::HalfPi;
		}
		else
		{
			Roll = asin( 2.F * SingularityTest );
		}
		return Roll * Math::RadToDegree;
	}

	inline float Quaternion::GetScalar() const
	{
		return w;
	}

	inline Vector3 Quaternion::GetVector() const
	{
		return Vector3( x, y, z );
	}

	inline Vector3 Quaternion::ToEulerAngles() const
	{
		Vector3 EulerFromQuat;

		float PitchY = std::asin( 2.F * (x * w - y * z) );
		float Test = std::cos( PitchY );
		if ( Test > Math::TendencyZero )
		{
			EulerFromQuat[ Roll ] = Math::Atan2( 2.F * (x * y + z * w), 1.F - (2.F * (Math::Square( z ) + Math::Square( x ))) ) * Math::RadToDegree;
			EulerFromQuat[ Pitch ] = PitchY * Math::RadToDegree;
			EulerFromQuat[ Yaw ] = Math::Atan2( 2.F * (z * x + y * w), 1.F - (2.F * (Math::Square( y ) + Math::Square( x ))) ) * Math::RadToDegree;
		}
		else
		{
			EulerFromQuat[ Roll ] = Math::Atan2( -2.F * (x * y - z * w), 1.F - (2.F * (Math::Square( y ) + Math::Square( z ))) ) * Math::RadToDegree;
			EulerFromQuat[ Pitch ] = PitchY * Math::RadToDegree;
			EulerFromQuat[ Yaw ] = 0.F;
		}

		if ( std::isinf( EulerFromQuat[ Roll ] ) || std::isnan( EulerFromQuat[ Roll ] ) )   EulerFromQuat[ Roll ] = 0.F;
		if ( std::isinf( EulerFromQuat[ Pitch ] ) || std::isnan( EulerFromQuat[ Pitch ] ) ) EulerFromQuat[ Pitch ] = 0.F;
		if ( std::isinf( EulerFromQuat[ Yaw ] ) || std::isnan( EulerFromQuat[ Yaw ] ) )     EulerFromQuat[ Yaw ] = 0.F;

		return EulerFromQuat;
	}

	FORCEINLINE float Quaternion::Dot( const Quaternion& other ) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	inline Quaternion Quaternion::Cross( const Quaternion& other ) const
	{
		return Quaternion(
			w * other.w - x * other.x - y * other.y - z * other.z,
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y + y * other.w + z * other.x - x * other.z,
			w * other.z + z * other.w + x * other.y - y * other.x
		);
	}

	inline const float* Quaternion::PointerToValue() const
	{
		return &w;
	}

	inline float& Quaternion::operator[]( unsigned char i )
	{
		return (&w)[ i ];
	}

	inline float const& Quaternion::operator[]( unsigned char i ) const
	{
		return (&w)[ i ];
	}

	FORCEINLINE bool Quaternion::operator==( const Quaternion& other ) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

	FORCEINLINE bool Quaternion::operator!=( const Quaternion& other ) const
	{
		return (x != other.x || y != other.y || z != other.z || w != other.w);
	}

	FORCEINLINE Quaternion Quaternion::operator-( void ) const
	{
		return Quaternion( -w, -x, -y, -z );
	}

	FORCEINLINE Quaternion Quaternion::operator*( const float& value ) const
	{
		return Quaternion( w * value, x * value, y * value, z * value );
	}

	FORCEINLINE Quaternion Quaternion::operator/( const float& value ) const
	{
		if ( value == 0.F ) Quaternion();
		return Quaternion( w / value, x / value, y / value, z / value );
	}

	FORCEINLINE Quaternion Quaternion::operator*( const Quaternion& other ) const
	{
		Quaternion Result;

		Result.x = w * other.x + x * other.w + y * other.z - z * other.y;
		Result.y = w * other.y + y * other.w + z * other.x - x * other.z;
		Result.z = w * other.z + z * other.w + x * other.y - y * other.x;
		Result.w = w * other.w - x * other.x - y * other.y - z * other.z;

		return Result;
	}

	inline Vector3 Quaternion::operator*( const Vector3& vector ) const
	{
		Vector3 const QuatVector( GetVector() );
		Vector3 const QV( Vector3::Cross( QuatVector, vector ) );
		Vector3 const QQV( Vector3::Cross( QuatVector, QV ) );

		return vector + ((QV * w) + QQV) * 2.F;
	}

	FORCEINLINE Quaternion& Quaternion::operator*=( const Quaternion& other )
	{
		*this = *this * other;
		return *this;
	}

	FORCEINLINE Quaternion& Quaternion::operator*=( const float& value )
	{
		w *= value;
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	FORCEINLINE Quaternion& Quaternion::operator/=( const float& value )
	{
		if ( value == 0.F ) w = x = y = z = 0;
		w /= value;
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}
}