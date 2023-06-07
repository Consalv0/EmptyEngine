
#include "Math/MathUtils.h"

namespace Math
{
	int32 SolveQuadratic( float x[ 2 ], float a, float b, float c )
	{
		if ( fabs( a ) < 1e-14F )
		{
			if ( fabs( b ) < 1e-14F )
			{
				if ( c == 0 )
					return -1;
				return 0;
			}
			x[ 0 ] = -c / b;
			return 1;
		}
		float dscr = b * b - 4 * a * c;
		if ( dscr > 0 )
		{
			dscr = sqrtf( dscr );
			x[ 0 ] = (-b + dscr) / (2 * a);
			x[ 1 ] = (-b - dscr) / (2 * a);
			return 2;
		}
		else if ( dscr == 0 )
		{
			x[ 0 ] = -b / (2 * a);
			return 1;
		}
		else
			return 0;
	}

	int32 SolveCubicNormed( float* x, float a, float b, float c )
	{
		float a2 = a * a;
		float q = (a2 - 3.F * b) / 9.F;
		float r = (a * (2.F * a2 - 9.F * b) + 27.F * c) / 54.F;
		float r2 = r * r;
		float q3 = q * q * q;
		if ( r2 < q3 )
		{
			float t = r / sqrtf( q3 );
			if ( t < -1.F ) t = -1.F;
			if ( t > 1.F ) t = 1.F;
			t = acosf( t );
			a /= 3.F; q = -2.F * sqrtf( q );
			x[ 0 ] = q * cosf( t / 3.F ) - a;
			x[ 1 ] = q * cosf( (t + 2.F * Math::Pi) / 3.F ) - a;
			x[ 2 ] = q * cosf( (t - 2.F * Math::Pi) / 3.F ) - a;
			return 3;
		}
		else
		{
			float a1, b1;
			a1 = -powf( fabs( r ) + sqrtf( r2 - q3 ), 1 / 3.F );
			if ( r < 0 ) a1 = -a1;
			b1 = a1 == 0 ? 0 : q / a1;
			a /= 3;
			x[ 0 ] = (a1 + b1) - a;
			x[ 1 ] = -0.5F * (a1 + b1) - a;
			x[ 2 ] = 0.5F * sqrtf( 3.F ) * (a1 - b1);
			if ( fabs( x[ 2 ] ) < 1e-14F )
				return 2;
			return 1;
		}
	}

	int32 SolveCubic( float x[ 3 ], float a, float b, float c, float d )
	{
		if ( fabs( a ) < 1e-14F )
			return SolveQuadratic( x, b, c, d );
		return SolveCubicNormed( x, b / a, c / a, d / a );
	}

	template<typename T>
	float Shoelace2( const T& a, const T& b )
	{
		return (b[ 0 ] - a[ 0 ]) * (a[ 1 ] + b[ 1 ]);
	}
}

namespace Math
{
	//* The number is power of 2
	inline int32 IsPow2( const int32 a )
	{
		return ((a & (a - 1)) == 0);
	}

	//* Get the next power2 of the value
	inline int32 NextPow2( int32 x )
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return ++x;
	}
}

template <typename T>
T Math::Min( const T& a, const T& b )
{
	return b < a ? b : a;
}

template <typename T>
T Math::Max( const T& a, const T& b )
{
	return a < b ? b : a;
}

template <typename T, typename S>
T Math::Median( const T& a, const T& b, const S& alpha )
{
	return Max( Min( a, b ), Min( Max( a, b ), alpha ) );
}

template <typename T, typename S>
T Math::Mix( const T& a, const T& b, const S& weight )
{
	return T( (S( 1 ) - weight) * a + weight * b );
}

template<typename T>
T Math::Abs( const T& value )
{
    return (value >= (T)0) ? value : -value;
}

template<typename T>
T Math::Sign( const T& value )
{
	return (T( 0 ) < value) - (value < T( 0 ));
}

template<typename T>
T Math::NonZeroSign( const T& value )
{
	return T( 2 ) * (value > T( 0 )) - T( 1 );
}

template<typename T>
T Math::Square( const T& value )
{
	return value * value;
}

template<typename T>
T Math::Map( const T& value, const T& minA, const T& maxA, const T& minB, const T& maxB )
{
	return minB + (value - minA) * (maxA - minB) / (maxB - minA);
}

template <typename T>
T Math::Clamp( const T& value, const T& a )
{
	return value >= T( 0 ) && value <= a ? value : T( value > T( 0 ) ) * a;
}

template <typename T>
T Math::Clamp( const T& value, const T& a, const T& b )
{
	return value >= a && value <= b ? value : value < a ? a : b;
}

template <typename T>
T Math::Clamp01( const T& value )
{
	return value >= T( 0 ) && value <= T( 1 ) ? value : value < T( 0 ) ? T( 0 ) : T( 1 );
}

float Math::ClampDegrees( float angle )
{
	angle = fmod( angle, 360.F );

	if ( angle < 0.F )
	{
		// --- Shift to [0,360)
		angle += 360.F;
	}

	return angle;
}

float Math::NormalizeAngle( float degrees )
{
	degrees = ClampDegrees( degrees );

	if ( degrees > 180.f )
	{
		// --- Shift to (-180,180]
		degrees -= 360.f;
	}

	return degrees;
}

float Math::Pow10( int32 number )
{
	float Ret = 1.0F;
	float r = 10.0F;
	if ( number < 0 )
	{
		number = -number;
		r = 0.1F;
	}

	while ( number )
	{
		if ( number & 1 )
		{
			Ret *= r;
		}
		r *= r;
		number >>= 1;
	}
	return Ret;
}

// Referenced from UE4 implementation
float Math::Atan2( float y, float x )
{
	const float absX = Math::Abs( x );
	const float absY = Math::Abs( y );
	const bool yAbsBigger = (absY > absX);
	float t0 = yAbsBigger ? absY : absX; // max(absY, absX)
	float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

	if ( t0 == 0.F )
		return 0.F;

	float t3 = t1 / t0;
	float t4 = t3 * t3;

	static const float c[ 7 ] = {
		+7.2128853633444123e-03F,
		-3.5059680836411644e-02F,
		+8.1675882859940430e-02F,
		-1.3374657325451267e-01F,
		+1.9856563505717162e-01F,
		-3.3324998579202170e-01F,
		+1.0F
	};

	t0 = c[ 0 ];
	t0 = t0 * t4 + c[ 1 ];
	t0 = t0 * t4 + c[ 2 ];
	t0 = t0 * t4 + c[ 3 ];
	t0 = t0 * t4 + c[ 4 ];
	t0 = t0 * t4 + c[ 5 ];
	t0 = t0 * t4 + c[ 6 ];
	t3 = t0 * t3;

	t3 = yAbsBigger ? (0.5F * Math::Pi) - t3 : t3;
	t3 = (x < 0.0F) ? Math::Pi - t3 : t3;
	t3 = (y < 0.0F) ? -t3 : t3;

	return t3;
}

float Math::Hypotenuse( float x, float y )
{
    return sqrtf( x * x + y * y );
}