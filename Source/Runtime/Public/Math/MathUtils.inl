
namespace EE::Math
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
    constexpr T Math::Min( const T& a, const T& b )
    {
        return b < a ? b : a;
    }

    template <typename T>
    constexpr T Math::Max( const T& a, const T& b )
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
    T Math::Sqrt( const T& value )
    {
        return std::sqrt( value );
    }

    template<typename T>
    T Math::Modulus( const T& a, const T& b )
    {
        return std::fmod( a, b );
    }

    template<typename T>
    T Math::Map( const T& value, const T& sourceMin, const T& sourceMax, const T& destMin, const T& destMax )
    {
        return destMin + (value - sourceMin) * (sourceMax - destMin) / (destMax - sourceMin);
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
        angle = Math::Modulus( angle, T( 360 ) );

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
        degrees = Math::ClampDegrees( degrees );

        if ( degrees > T( 180 ) )
        {
            // --- Shift to (-180,180]
            degrees -= T( 360 );
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

    template <typename T>
    T Math::Atan2( const T& y, const T& x )
    {
        return std::atan2( y, x );
    }

    template <typename T>
    T Math::Asin( const T& radians )
    {
        return std::asin( radians );
    }

    template <typename T>
    T Math::Sin( const T& radians )
    {
        return std::sin( radians );
    }

    template <typename T>
    T Math::Cos( const T& radians )
    {
        return std::cos( radians );
    }

    template <typename T>
    T Math::Tan( const T& radians )
    {
        return std::tan( radians );
    }

    template <typename T>
    T Math::ATan( const T& value )
    {
        return std::atan( value );
    }

    template <typename T>
    T Math::ATan2( const T& x, const T& y )
    {
        return std::atan2( x, y );
    }

    template <typename T>
    T Math::Acos( const T& radians )
    {
        return std::acos( radians );
    }

    template <typename T>
    T Math::Hypotenuse( const T& x, const T& y )
    {
        return std::hypot( x, y );
    }

    template<typename T>
    bool EE::Math::IsInfiniteOrNan( const T& value )
    {
        return std::isinf( value ) || std::isnan( value );
    }

    template<typename T>
    T EE::Math::Ceil( const T& value )
    {
        return std::ceil( value );
    }
}