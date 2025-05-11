#pragma once

namespace EE
{
    template<unsigned Frac>
    struct MathConstants<fixed<Frac>>
    {
        static constexpr fixed<Frac> Pi = fixed<Frac>::FromRaw( FIXED_PI( Frac ) );
        static constexpr fixed<Frac> QuarterPi = fixed<Frac>::FromRaw( FIXED_QUARTER_PI( Frac ) );
        static constexpr fixed<Frac> Epsilon = fixed<Frac>::FromRaw( FIXED_EPSILON( Frac ) );
        static constexpr fixed<Frac> RadToDegree = fixed<Frac>( 180.0 / MathConstants<double>::Pi );
        static constexpr fixed<Frac> DegToRad = fixed<Frac>( MathConstants<double>::Pi / 180.0 );
        static constexpr fixed<Frac> DeltaPrecision = fixed<Frac>::FromRaw( 1 );
        static constexpr fixed<Frac> Ra = fixed<Frac>::FromRaw( FIXED_EPSILON( Frac ) );
        static constexpr fixed<Frac> MaxValue = fixed<Frac>::FromRaw( FIXED_MAX );
        static constexpr fixed<Frac> MinValue = fixed<Frac>::FromRaw( FIXED_MIN );
    };
    template <class T>
    inline constexpr bool IsFixedOrFloatingPoint =
        std::disjunction_v<
            std::is_same<std::remove_cv_t<T>, float>,
            std::is_same<std::remove_cv_t<T>, double>,
            std::is_same<std::remove_cv_t<T>, long double>,
            std::is_same<std::remove_cv_t<T>, fixed<1>>,
            std::is_same<std::remove_cv_t<T>, fixed<2>>,
            std::is_same<std::remove_cv_t<T>, fixed<3>>,
            std::is_same<std::remove_cv_t<T>, fixed<4>>,
            std::is_same<std::remove_cv_t<T>, fixed<5>>,
            std::is_same<std::remove_cv_t<T>, fixed<6>>,
            std::is_same<std::remove_cv_t<T>, fixed<7>>,
            std::is_same<std::remove_cv_t<T>, fixed<8>>,
            std::is_same<std::remove_cv_t<T>, fixed<9>>,
            std::is_same<std::remove_cv_t<T>, fixed<10>>,
            std::is_same<std::remove_cv_t<T>, fixed<11>>,
            std::is_same<std::remove_cv_t<T>, fixed<12>>,
            std::is_same<std::remove_cv_t<T>, fixed<13>>,
            std::is_same<std::remove_cv_t<T>, fixed<14>>,
            std::is_same<std::remove_cv_t<T>, fixed<15>>,
            std::is_same<std::remove_cv_t<T>, fixed<16>>,
            std::is_same<std::remove_cv_t<T>, fixed<17>>,
            std::is_same<std::remove_cv_t<T>, fixed<18>>,
            std::is_same<std::remove_cv_t<T>, fixed<19>>,
            std::is_same<std::remove_cv_t<T>, fixed<20>>,
            std::is_same<std::remove_cv_t<T>, fixed<21>>,
            std::is_same<std::remove_cv_t<T>, fixed<22>>,
            std::is_same<std::remove_cv_t<T>, fixed<23>>,
            std::is_same<std::remove_cv_t<T>, fixed<24>>,
            std::is_same<std::remove_cv_t<T>, fixed<25>>,
            std::is_same<std::remove_cv_t<T>, fixed<26>>,
            std::is_same<std::remove_cv_t<T>, fixed<27>>,
            std::is_same<std::remove_cv_t<T>, fixed<28>>,
            std::is_same<std::remove_cv_t<T>, fixed<29>>,
            std::is_same<std::remove_cv_t<T>, fixed<30>>,
            std::is_same<std::remove_cv_t<T>, fixed<31>>
        >;
}

namespace EE::Math
{
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

    template<unsigned Frac>
    inline fixed<Frac> Ceil( const fixed<Frac>& value );

    // template <typename T>
    // inline T Hypotenuse( T x, T y );
}

#include "Math/Fixmath/FixmathUtils.inl"