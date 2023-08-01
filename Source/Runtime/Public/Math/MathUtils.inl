
#include "Math/MathUtils.h"

namespace EE::Math
{
    template<typename T>
	int32 SolveQuadratic( T x[ 2 ], T a, T b, T c )
	{
		if ( fabs( a ) < T(1e-14) )
		{
			if ( fabs( b ) < T(1e-14) )
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

    template<typename T>
	int32 SolveCubicNormed( T* x, T a, T b, T c )
	{
		float a2 = a * a;
		float q = (a2 - T(3) * b) / T(9);
		float r = (a * (T(2) * a2 - T(9) * b) + T(27) * c) / T(54);
		float r2 = r * r;
		float q3 = q * q * q;
		if ( r2 < q3 )
		{
			float t = r / std::sqrt( q3 );
			if ( t < T(-1) ) t = T(-1);
			if ( t > T(1) ) t = T(1);
			t = std::acos( t );
			a /= T(3); q = T(-2) * std::sqrt(q);
			x[ 0 ] = q * cosf( t / T(3) ) - a;
			x[ 1 ] = q * cosf( (t + T(3) * MathConstants<T>::Pi) / T(3) ) - a;
			x[ 2 ] = q * cosf( (t - T(3) * MathConstants<T>::Pi) / T(3) ) - a;
			return 3;
		}
		else
		{
			float a1, b1;
			a1 = -powf( fabs( r ) + std::sqrt( r2 - q3 ), 1 / T(3) );
			if ( r < 0 ) a1 = -a1;
			b1 = a1 == 0 ? 0 : q / a1;
			a /= 3;
			x[ 0 ] = (a1 + b1) - a;
			x[ 1 ] = T(-0.5) * (a1 + b1) - a;
			x[ 2 ] = T(0.5) * sqrtf( T(3) ) * (a1 - b1);
			if ( fabs( x[ 2 ] ) < 1e-14F )
				return 2;
			return 1;
		}
	}

    template<typename T>
	int32 SolveCubic( T x[ 3 ], T a, T b, T c, T d )
	{
		if ( fabs( a ) < T(1e-14) )
			return SolveQuadratic<T>( x, b, c, d );
		return SolveCubicNormed<T>( x, b / a, c / a, d / a );
	}

	template<typename T>
	T Shoelace2( const T& a, const T& b )
	{
		return (b[ 0 ] - a[ 0 ]) * (a[ 1 ] + b[ 1 ]);
	}
}

namespace Math
{
	//* The number is power of 2
    template<typename T>
	inline T IsPow2( const T a )
	{
		return ((a & (a - 1)) == 0);
	}

	//* Get the next power2 of the value
    template<typename T>
	inline T NextPow2( T x )
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

namespace EE
{
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

    template <typename T>
    T Math::ClampDegrees( T angle )
    {
        angle = fmod( angle, T( 360 ) );

        if ( angle < 0 )
        {
            // --- Shift to [0,360)
            angle += T( 360 );
        }

        return angle;
    }

    template <typename T>
    T Math::NormalizeDegrees( T degrees )
    {
        degrees = ClampDegrees( degrees );

        if ( degrees > 180.f )
        {
            // --- Shift to (-180,180]
            degrees -= 360.f;
        }

        return degrees;
    }

    template <typename R, typename T>
    R Math::Pow10( T number )
    {
        R Ret = R( 1 );
        R r = R( 10 );
        if ( number < T( 0 ) )
        {
            number = -number;
            r = R( 0.1 );
        }

        while ( number )
        {
            if ( number & T( 1 ) )
            {
                Ret *= r;
            }
            r *= r;
            number >>= T( 1 );
        }
        return Ret;
    }

    // Referenced from UE4 implementation
    template <typename T>
    T Math::Atan2( T y, T x )
    {
        return atan( y, x );
    }

    template <typename T>
    T Math::Hypotenuse( T x, T y )
    {
        return sqrt( x * x + y * y );
    }
}