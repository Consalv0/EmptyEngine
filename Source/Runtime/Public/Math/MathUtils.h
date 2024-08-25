#pragma once

#include <math.h>
#include <algorithm>

namespace EE
{
    template<typename T>
    struct MathConstants;

    template<>
    struct MathConstants<float>
    {
        static constexpr float Pi = 3.1415926535897932F;
        static constexpr float Tau = 6.2831853071795864F;
        static constexpr float TendencyZero = 1e-6F;
        static constexpr float Epsilon = FLT_EPSILON;
        static constexpr float MaxValue = FLT_MAX;
        static constexpr float MinValue = FLT_MIN;
        static constexpr float Euler = 2.7182817F;
        static constexpr float DeltaPrecision = 0.00001F;
        static constexpr float InversePi = 0.31830988618F;
        static constexpr float HalfPi = 0.5 * Pi;
        static constexpr float RadToDegree = 180.0F / Pi;
        static constexpr float DegToRad = Pi / 180.0F;
        static constexpr float SquareRoot2 = 1.41421356237F;
    };
    typedef MathConstants<float> FMathConstants;

    template<>
    struct MathConstants<double>
    {
        static constexpr double Pi = 3.1415926535897932384626433832795;
        static constexpr double Tau = 6.2831853071795864769252867665590;
        static constexpr double TendencyZero = 1e-08;
        static constexpr double Epsilon = DBL_EPSILON;
        static constexpr double MaxValue = DBL_MAX;
        static constexpr double MinValue = DBL_MIN;
        static constexpr double Euler = 2.7182818284590452353602874713527;
        static constexpr double InversePi = 0.318309886183790671537767526745;
        static constexpr double HalfPi = 0.5 * Pi;
        static constexpr double DeltaPrecision = 0.0000001F;
        static constexpr double RadToDegree = 180.0 / Pi;
        static constexpr double DegToRad = Pi / 180.0;
        static constexpr double SquareRoot2 = 1.4142135623730950488016887242097;
    };
    typedef MathConstants<double> DMathConstants;

    template<>
    struct MathConstants<int32>
    {
        static constexpr int32 MaxValue = INT32_MAX;
        static constexpr int32 MinValue = INT32_MIN;
    };

    template<>
    struct MathConstants<int64>
    {
        static constexpr int64 MaxValue = INT64_MAX;
        static constexpr int64 MinValue = INT64_MIN;
    };

    template<>
    struct MathConstants<uint32>
    {
        static constexpr int64 MaxValue = UINT32_MAX;
        static constexpr int64 MinValue = 0;
    };

    template<>
    struct MathConstants<uint64>
    {
        static constexpr int64 MaxValue = UINT64_MAX;
        static constexpr int64 MinValue = 0;
    };
}

namespace EE::Math
{
	//* Returns the smaller of the arguments.
	template <typename T>
    FORCEINLINE constexpr T Min( const T& a, const T& b );

	//* Returns the larger of the arguments.
	template <typename T>
    FORCEINLINE constexpr T Max( const T& a, const T& b );

	//* Returns the middle out of three values
	template <typename T, typename S>
	inline T Median( const T& a, const T& b, const S& alpha );

	//* Returns the weighted average of a and b.
	template <typename T, typename S>
	inline T Mix( const T& a, const T& b, const S& weight );

	//* Get the absolute value
	template <typename T>
	inline T Abs( const T& value );

	//* Returns 1 for positive values, -1 for negative values, and 0 for zero.
	template <typename T>
	inline T Sign( const T& value );

	/// Returns 1 for non-negative values and -1 for negative values.
	template <typename T>
	inline T NonZeroSign( const T& value );

    template <typename T>
    inline T Square( const T& value );

    //* Square root
    template <typename T>
    inline T Sqrt( const T& value );

    //* Returns remainder after division
    template <typename T>
    inline T Modulus( const T& a, const T& b );

	//* Remap the value to another range of values
	template <typename T>
    inline T Map( const T& value, const T& sourceMin, const T& sourceMax, const T& destMin, const T& destMax );

	//* Clamps the number to the interval from 0 to b.
	template <typename T>
	inline T Clamp( const T& value, const T& a );

	//* Clamp the value in the defined range 
	template <typename T>
	inline T Clamp( const T& value, const T& a, const T& b );

	//* Clamp the value in range of [0, 1] 
	template <typename T>
	inline T Clamp01( const T& value );

	//* Get the angle in degrees in range of [0, 360)
    template <typename T>
	inline T ClampDegrees( T Degrees );

	//* Get the angle in degrees in the range of (-180, 180]
    template <typename T>
	inline T NormalizeDegrees( T Degrees );

	//* Fast pow to ten
    template <typename R, typename T>
    inline R Pow10( T number );

	// Error consideration
    template <typename T>
	inline T Atan2( const T& y, const T& x );

    // Arc sine function
    template <typename T>
    inline T Asin( const T& value );

    // Sine function
    template <typename T>
    inline T Sin( const T& value );

    // Cosine function
    template <typename T>
    inline T Cos( const T& value );

    // Tangent function
    template <typename T>
    inline T Tan( const T& value );

    // Arc Tangent function
    template <typename T>
    inline T ATan( const T& value );

    // Arc Tangent x,y function
    template <typename T>
    inline T ATan2( const T& x, const T& y );

    // Arc Cosine function
    template <typename T>
    inline T Acos( const T& value );

    template <typename T>
    inline T Hypotenuse( const T& x, const T& y );

    template <typename T>
    inline bool IsInfiniteOrNan( const T& value );

    template <typename T>
    inline T Ceil( const T& value );
}

#include "Math/MathUtils.inl"
