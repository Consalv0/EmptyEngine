#pragma once

#include <math.h>
#include <stdexcept>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3.h"
#include "Math/Quaternion.h"

namespace EE::Math
{
    template <typename T>
	FORCEINLINE TQuaternion<T>::TQuaternion()
		:w( 1 ), x( 0 ), y( 0 ), z( 0 )
	{
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>::TQuaternion( TQuaternion<T> const& other )
		: w( other.w ), x( other.x ), y( other.y ), z( other.z )
	{
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>::TQuaternion( T const& Scale, TVector3<T> const& vector )
		: w( Scale ), x( vector.x ), y( vector.y ), z( vector.z )
	{
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>::TQuaternion( T const& w, T const& x, T const& y, T const& z )
		: w( w ), x( x ), y( y ), z( z )
	{
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::FromEulerAngles( TVector3<T> const& EulerAngles )
	{
		const T Scale = MathConstants<T>::DegToRad * T(0.5);
		T HalfRoll = EulerAngles[ Roll ] * Scale;
		T HalfPitch = EulerAngles[ Pitch ] * Scale;
		T HalfYaw = EulerAngles[ Yaw ] * Scale;

		T SR = Math::Sin( HalfRoll );
		T CR = Math::Cos( HalfRoll );
		T SP = Math::Sin( HalfPitch );
		T CP = Math::Cos( HalfPitch );
		T SY = Math::Sin( HalfYaw );
		T CY = Math::Cos( HalfYaw );

		TQuaternion EulerToQuat;
		EulerToQuat.x = (CY * SP * CR) + (SY * CP * SR);
		EulerToQuat.y = (SY * CP * CR) - (CY * SP * SR);
		EulerToQuat.z = (CY * CP * SR) - (SY * SP * CR);
		EulerToQuat.w = (CY * CP * CR) + (SY * SP * SR);
		return EulerToQuat;
	}

    template <typename T>
	FORCEINLINE TQuaternion<T> TQuaternion<T>::FromToRotation( TVector3<T> const& from, TVector3<T> const& to )
	{
		TVector3 Half = from + to;
		Half.Normalize();

		return TQuaternion(
			from.Dot( Half ),
			from.y * Half.z - from.z * Half.y,
			from.z * Half.x - from.x * Half.z,
			from.x * Half.y - from.y * Half.x
		).Normalized();
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::FromAxisAngle( TVector3<T> const& Axis, T const& Radians )
	{
		T Sine = Math::Sin( Radians * T(.5) );

		return TQuaternion(
			Math::Cos( Radians * T(.5) ),
			Axis.x * Sine,
			Axis.y * Sine,
			Axis.z * Sine
		);
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::FromLookRotation( TVector3<T> const& forward, TVector3<T> const& up )
	{
		const TVector3<T> normal = forward.Normalized();
		const TVector3<T> tangent = TVector3::Cross( up == normal ? up + T(0.001F) : up, normal ).Normalized();
		const TVector3<T> Bitangent = TVector3::Cross( normal, tangent );

		TMatrix3x3 LookSpace(
			tangent, Bitangent, normal
		);

		return TQuaternion::FromMatrix( LookSpace );
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::FromMatrix( TMatrix3x3<T> const& matrix )
	{
		const T tr = matrix.m00 + matrix.m11 + matrix.m22;

		if ( tr > T(0) )
		{
			T num0 = Math::Sqrt( tr + T(1) );
			T num1 = T(.5) / num0;
			return TQuaternion(
				num0 * T(.5),
				(matrix.m12 - matrix.m21) * num1,
				(matrix.m20 - matrix.m02) * num1,
				(matrix.m01 - matrix.m10) * num1
			);
		}
		if ( (matrix.m00 >= matrix.m11) && (matrix.m00 >= matrix.m22) )
		{
            T num7 = Math::Sqrt( ((T(1) + matrix.m00) - matrix.m11) - matrix.m22 );
			T num4 = T(.5) / num7;
			return TQuaternion(
				(matrix.m12 - matrix.m21) * num4,
				T(.5) * num7,
				(matrix.m01 + matrix.m10) * num4,
				(matrix.m02 + matrix.m20) * num4
			);
		}
		if ( matrix.m11 > matrix.m22 )
		{
			T num6 = Math::Sqrt( ((T(1) + matrix.m11) - matrix.m00) - matrix.m22 );
			T num3 = T(.5) / num6;
			return TQuaternion(
				(matrix.m20 - matrix.m02) * num3,
				(matrix.m10 + matrix.m01) * num3,
				T(.5) * num6,
				(matrix.m21 + matrix.m12) * num3
			);
		}

		T num5 = Math::Sqrt( ((T(1) + matrix.m22) - matrix.m00) - matrix.m11 );
		T num2 = T(.5) / num5;
		return TQuaternion(
			(matrix.m01 - matrix.m10) * num2,
			(matrix.m20 + matrix.m02) * num2,
			(matrix.m21 + matrix.m12) * num2,
			T(.5) * num5
		);
	}

    template <typename T>
	inline void TQuaternion<T>::Interpolate( TQuaternion<T>& out, const TQuaternion<T>& start, const TQuaternion<T>& end, T factor )
	{
		T cosTheta = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

		// Adjust signs (if necessary)
		TQuaternion adjEnd = end;
		if ( cosTheta < T(0) )
		{
			cosTheta = -cosTheta;
			adjEnd.x = -adjEnd.x;   // Reverse all signs
			adjEnd.y = -adjEnd.y;
			adjEnd.z = -adjEnd.z;
			adjEnd.w = -adjEnd.w;
		}

		// Calculate coefficients
		T sclp, sclq;
		if ( (T(1) - cosTheta) > T(0.0001) )
		{
			// Standard case (slerp)
			T omega = Math::Acos( cosTheta ); // extract theta from dot product's cos theta
			T sinom = Math::Sin( omega );
			sclp = Math::Sin( (1 - factor) * omega ) / sinom;
			sclq = Math::Sin( factor * omega ) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = T(1) - factor;
			sclq = factor;
		}

		out.x = sclp * start.x + sclq * adjEnd.x;
		out.y = sclp * start.y + sclq * adjEnd.y;
		out.z = sclp * start.z + sclq * adjEnd.z;
		out.w = sclp * start.w + sclq * adjEnd.w;
	}

    template <typename T>
	inline T TQuaternion<T>::Magnitude() const
	{
		return Math::Sqrt( x * x + y * y + z * z + w * w );
	}

    template <typename T>
	inline T TQuaternion<T>::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

    template <typename T>
	inline void TQuaternion<T>::Normalize()
	{
		if ( MagnitudeSquared() == T(0) )
		{
			w = T(1); x = T(0); y = T(0); z = T(0);
		}
		else
		{
			*this /= Magnitude();
		}
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::Normalized() const
	{
		if ( MagnitudeSquared() == T(0) ) return TQuaternion();
		TQuaternion Result = TQuaternion( *this );
		return Result /= Magnitude();
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::Conjugated() const
	{
		return TQuaternion( GetScalar(), GetVector() * T(-1) );
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::Inversed() const
	{
		T absoluteValue = Magnitude();
		absoluteValue *= absoluteValue;
		absoluteValue = T(1) / absoluteValue;

		TQuaternion ConjugateVal = Conjugated();

		T Scalar = ConjugateVal.GetScalar() * absoluteValue;
		TVector3<T> Imaginary = ConjugateVal.GetVector() * absoluteValue;

		return TQuaternion( Scalar, Imaginary );
	}

    template <typename T>
	inline TMatrix4x4<T> TQuaternion<T>::ToMatrix4x4() const
	{
		TMatrix4x4<T> result;
		T xx( x * x );
		T yy( y * y );
		T zz( z * z );
		T xz( x * z );
		T xy( x * y );
		T yz( y * z );
		T wx( w * x );
		T wy( w * y );
		T wz( w * z );

		result.m00 = T(1) - T(2) * (yy + zz);
		result.m01 = T(2) * (xy + wz);
		result.m02 = T(2) * (xz - wy);

		result.m10 = T(2) * (xy - wz);
		result.m11 = T(1) - T(2) * (xx + zz);
		result.m12 = T(2) * (yz + wx);

		result.m20 = T(2) * (xz + wy);
		result.m21 = T(2) * (yz - wx);
		result.m22 = T(1) - T(2) * (xx + yy);
		return result;
	}

    template <typename T>
	inline T TQuaternion<T>::GetPitch() const
	{
		T pitch;
		const T singularityTest = x * y + z * w;
		if ( Math::Abs( singularityTest ) > T(0.499995) )
		{
			return 0;
		}
		else
		{
            pitch = Math::Atan2( (T(2) * x * w) - (T(2) * y * z), T(1) - (T(2) * Math::Square( x )) - (T(2) * Math::Square( z )) );
		}
		return pitch * MathConstants<T>::RadToDegree;
	}

    template <typename T>
	inline T TQuaternion<T>::GetYaw() const
	{
		T yaw;
		const T SingularityTest = x * y + z * w;
        if ( SingularityTest > T(0.499995) )
		{
            yaw = T(2) * Math::Atan2( x, w );
		}
		else if ( SingularityTest < T(-0.49999) )
		{
			yaw = T(-2) * Math::Atan2(x, w);
		}
		else
		{
			const T sqy = y * y;
			const T sqz = z * z;
            yaw = Math::Atan2( (T(2) * y * w) - (T(2) * x * z), T(1) - (T(2) * sqy) - (T(2) * sqz) );
		}
		return yaw * MathConstants<T>::RadToDegree;
	}

    template <typename T>
	inline T TQuaternion<T>::GetRoll() const
	{
		T roll;
		const T singularityTest = x * y + z * w;
        if ( singularityTest > T(0.499995) )
		{
			roll = MathConstants<T>::HalfPi;
		}
		else if ( singularityTest < T(-0.499995) )
		{
			roll = -MathConstants<T>::HalfPi;
		}
		else
		{
            roll = asin( T(2) * singularityTest );
		}
		return roll * MathConstants<T>::RadToDegree;
	}

    template <typename T>
	inline T TQuaternion<T>::GetScalar() const
	{
		return scalar;
	}

    template <typename T>
	inline TVector3<T> TQuaternion<T>::GetVector() const
	{
		return vector;
	}

    template <typename T>
	inline TVector3<T> TQuaternion<T>::ToEulerAngles() const
	{
		TVector3<T> eulerFromQuat;

		T PitchY = std::asin( T(2) * (x * w - y * z) );
		T Test = std::cos( PitchY );
		if ( Test > MathConstants<T>::TendencyZero )
		{
			eulerFromQuat[ Roll ] = Math::Atan2( T(2) * (x * y + z * w), T(1) - (T(2) * (Math::Square( z ) + Math::Square( x ))) ) * MathConstants<T>::RadToDegree;
			eulerFromQuat[ Pitch ] = PitchY * MathConstants<T>::RadToDegree;
			eulerFromQuat[ Yaw ] = Math::Atan2( T(2) * (z * x + y * w), T(1) - (T(2) * (Math::Square( y ) + Math::Square( x ))) ) * MathConstants<T>::RadToDegree;
		}
		else
		{
			eulerFromQuat[ Roll ] = Math::Atan2( T(-2.F) * (x * y - z * w), T(1) - (T(2) * (Math::Square( y ) + Math::Square( z ))) ) * MathConstants<T>::RadToDegree;
			eulerFromQuat[ Pitch ] = PitchY * MathConstants<T>::RadToDegree;
			eulerFromQuat[ Yaw ] = T(0);
		}

		if ( Math::IsInfiniteOrNan( eulerFromQuat[ Roll ] ) )   eulerFromQuat[ Roll ] = T(0);
		if ( Math::IsInfiniteOrNan( eulerFromQuat[ Pitch ] ) ) eulerFromQuat[ Pitch ] = T(0);
		if ( Math::IsInfiniteOrNan( eulerFromQuat[ Yaw ] ) )     eulerFromQuat[ Yaw ] = T(0);

		return eulerFromQuat;
	}

    template <typename T>
	FORCEINLINE T TQuaternion<T>::Dot( const TQuaternion<T>& other ) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

    template <typename T>
	inline TQuaternion<T> TQuaternion<T>::Cross( const TQuaternion& other ) const
	{
		return TQuaternion(
			w * other.w - x * other.x - y * other.y - z * other.z,
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y + y * other.w + z * other.x - x * other.z,
			w * other.z + z * other.w + x * other.y - y * other.x
		);
	}

    template <typename T>
	inline const T* TQuaternion<T>::PointerToValue() const
	{
		return &w;
	}

    template <typename T>
	inline T& TQuaternion<T>::operator[]( unsigned char i )
	{
		return (&w)[ i ];
	}

    template <typename T>
	inline T const& TQuaternion<T>::operator[]( unsigned char i ) const
	{
		return (&w)[ i ];
	}

    template <typename T>
	FORCEINLINE bool TQuaternion<T>::operator==( const TQuaternion<T>& other ) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

    template <typename T>
	FORCEINLINE bool TQuaternion<T>::operator!=( const TQuaternion<T>& other ) const
	{
		return (x != other.x || y != other.y || z != other.z || w != other.w);
	}

    template <typename T>
	FORCEINLINE TQuaternion<T> TQuaternion<T>::operator-( void ) const
	{
		return TQuaternion( -w, -x, -y, -z );
	}

    template <typename T>
	FORCEINLINE TQuaternion<T> TQuaternion<T>::operator*( const T& value ) const
	{
		return TQuaternion( w * value, x * value, y * value, z * value );
	}

    template <typename T>
	FORCEINLINE TQuaternion<T> TQuaternion<T>::operator/( const T& value ) const
	{
		if ( value == 0 ) TQuaternion();
		return TQuaternion( w / value, x / value, y / value, z / value );
	}

    template <typename T>
	FORCEINLINE TQuaternion<T> TQuaternion<T>::operator*( const TQuaternion& other ) const
	{
		TQuaternion result;

		result.x = w * other.x + x * other.w + y * other.z - z * other.y;
		result.y = w * other.y + y * other.w + z * other.x - x * other.z;
		result.z = w * other.z + z * other.w + x * other.y - y * other.x;
		result.w = w * other.w - x * other.x - y * other.y - z * other.z;

		return result;
	}

    template <typename T>
	inline TVector3<T> TQuaternion<T>::operator*( const TVector3<T>& vector ) const
	{
		TVector3<T> const quatVector( GetVector() );
		TVector3<T> const QV( TVector3<T>::Cross( quatVector, vector ) );
		TVector3<T> const QQV( TVector3<T>::Cross( quatVector, QV ) );

		return vector + ((QV * w) + QQV) * T(2);
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>& TQuaternion<T>::operator*=( const TQuaternion& other )
	{
		*this = *this * other;
		return *this;
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>& TQuaternion<T>::operator*=( const T& value )
	{
		w *= value;
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

    template <typename T>
	FORCEINLINE TQuaternion<T>& TQuaternion<T>::operator/=( const T& value )
	{
		if ( value == T(0) ) w = x = y = z = T(0);
		w /= value;
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}
}