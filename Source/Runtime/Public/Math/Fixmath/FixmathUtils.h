#pragma once

namespace EE
{
    template<unsigned Frac>
    struct MathConstants<fixed<Frac>>
    {
        static constexpr fixed<Frac> Pi = fixed<Frac>::FromRaw( FIXED_PI( Frac ) );
        static constexpr fixed<Frac> QuarterPi = fixed<Frac>::FromRaw( FIXED_QUARTER_PI( Frac ) );
        static constexpr fixed<Frac> Epsilon = fixed<Frac>::FromRaw( FIXED_EPSILON( Frac ) );
        static constexpr fixed<Frac> MaxValue = fixed<Frac>::FromRaw( FIXED_MAX );
        static constexpr fixed<Frac> MinValue = fixed<Frac>::FromRaw( FIXED_MIN );
    };

    template <class T>
    inline constexpr bool IsFixedOrFloatingPoint =
        std::_Is_any_of_v<std::remove_cv_t<T>, float, double, long double,
        fixed<1>, fixed<2>, fixed<3>, fixed<4>, fixed<5>, fixed<6>, fixed<7>, fixed<8>,
        fixed<9>, fixed<10>, fixed<11>, fixed<12>, fixed<13>, fixed<14>, fixed<15>, fixed<16>,
        fixed<17>, fixed<18>, fixed<19>, fixed<20>, fixed<21>, fixed<22>, fixed<23>, fixed<24>,
        fixed<25>, fixed<26>, fixed<27>, fixed<28>, fixed<29>, fixed<30>, fixed<31>>;
}

namespace EE::Math
{
    //* Returns the smaller of the arguments.
    template<unsigned Frac>
    inline fixed<Frac> Min( const fixed<Frac>& a, const fixed<Frac>& b );

    //* Returns the larger of the arguments.
    template<unsigned Frac>
    inline fixed<Frac> Max( const fixed<Frac>& a, const fixed<Frac>& b );
     
    // //* Returns the middle out of three values
    // template <typename T, typename S>
    // inline T Median( const T& a, const T& b, const S& alpha );
    // 
    // //* Returns the weighted average of a and b.
    // template <typename T, typename S>
    // inline T Mix( const T& a, const T& b, const S& weight );
    // 
    // //* Get the absolute value
    // template <typename T>
    // inline T Abs( const T& value );
    // 
    // //* Returns 1 for positive values, -1 for negative values, and 0 for zero.
    // template <typename T>
    // inline T Sign( const T& value );
    // 
    // /// Returns 1 for non-negative values and -1 for negative values.
    // template <typename T>
    // inline T NonZeroSign( const T& value );

    template<unsigned Frac>
    inline fixed<Frac> Square( const fixed<Frac>& value );

    //* Square root
    template<unsigned Frac>
    inline fixed<Frac> Sqrt( const fixed<Frac>& value );

    //* Returns remainder after division
    template<unsigned Frac>
    inline fixed<Frac> Modulus( const fixed<Frac>& a, const fixed<Frac>& b );
    
    // //* Remap the value to another range of values
    // template <typename T>
    // inline T Map( const T& value, const T& minA, const T& maxA, const T& minB, const T& maxB );
    // 
    // //* Clamps the number to the interval from 0 to b.
    // template <typename T>
    // inline T Clamp( const T& value, const T& a );
    // 
    // //* Clamp the value in the defined range 
    // template <typename T>
    // inline T Clamp( const T& value, const T& a, const T& b );
    // 
    // //* Clamp the value in range of [0, 1] 
    // template <typename T>
    // inline T Clamp01( const T& value );
    // 
    // //* Get the angle in degrees in range of [0, 360)
    // template <typename T>
    // inline T ClampDegrees( T Degrees );
    // 
    // //* Get the angle in degrees in the range of (-180, 180]
    // template <typename T>
    // inline T NormalizeDegrees( T Degrees );
    // 
    // //* Fast pow to ten
    // template <typename R, typename T>
    // inline R Pow10( T number );
    // 
    // //* Error consideration
    // template <typename T>
    // inline T Atan2( T y, T x );
    // 
    template<unsigned Frac>
    inline fixed<Frac> Sin( const fixed<Frac>& radians );

    template<unsigned Frac>
    inline fixed<Frac> Cos( const fixed<Frac>& radians );

    template<unsigned Frac>
    inline fixed<Frac> Tan( const fixed<Frac>& radians );

    template<unsigned Frac>
    inline fixed<Frac> ATan( const fixed<Frac>& value );

    template<unsigned Frac>
    inline fixed<Frac> ATan2( const fixed<Frac>& x, const fixed<Frac>& y );
    // template <typename T>
    // inline T Hypotenuse( T x, T y );
}

#include "Math/Fixmath/FixmathUtils.inl"