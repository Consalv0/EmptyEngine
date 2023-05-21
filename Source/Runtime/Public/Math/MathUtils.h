#pragma once

#include <math.h>
#include <algorithm>

namespace Math
{
	static constexpr float Pi = 3.1415926535897932F;
	static constexpr float TendencyZero = 1e-6F;
	static constexpr float Epsilon = FLT_EPSILON;
	static constexpr float MaxNumber = FLT_MAX;
	static constexpr float Euler = 2.71828182845904523536F;
	static constexpr float InversePi = 0.31830988618F;
	static constexpr float HalfPi = 1.57079632679F;
	static constexpr float DeltaPresicion = 0.00001F;
	static constexpr float RadToDegree = 57.2957795130823208F;
	static constexpr float DegToRad = 0.0174532925199432F;
	static constexpr float SquareRoot2 = 1.41421356237F;
}

namespace Math
{
	inline int SolveQuadratic( float x[ 2 ], float a, float b, float c );

	inline int SolveCubicNormed( float* x, float a, float b, float c );

	inline int SolveCubic( float x[ 3 ], float a, float b, float c, float d );

	template <typename T>
	inline float Shoelace2( const T& a, const T& b );
}

namespace Math
{
	//* Returns the smaller of the arguments.
	template <typename T>
	inline T Min( const T a, const T b );

	//* Returns the larger of the arguments.
	template <typename T>
	inline T Max( const T a, const T b );

	//* Returns the middle out of three values
	template <typename T, typename S>
	inline T Median( const T a, const T b, const S alpha );

	//* Returns the weighted average of a and b.
	template <typename T, typename S>
	inline T Mix( const T a, const T b, const S weight );

	//* Get the absolute value
	template <typename T>
	inline T Abs( const T value );

	//* Returns 1 for positive values, -1 for negative values, and 0 for zero.
	template <typename T>
	inline T Sign( const T value );

	/// Returns 1 for non-negative values and -1 for negative values.
	template <typename T>
	inline T NonZeroSign( const T value );

	template <typename T>
	inline T Square( const T value );

	//* Remap the value to another range of values
	template <typename T>
	inline T Map( const T value, const T minA, const T maxA, const T minB, const T maxB );

	//* Clamps the number to the interval from 0 to b.
	template <typename T>
	inline T Clamp( const T value, const T a );

	//* Clamp the value in the defined range 
	template <typename T>
	inline T Clamp( const T value, const T a, const T b );

	//* Clamp the value in range of [0, 1] 
	template <typename T>
	inline T Clamp01( const T value );

	//* Get the angle in degrees in range of [0, 360)
	inline float ClampDegrees( float Degrees );

	//* Get the angle in degrees in the range of (-180, 180]
	inline float NormalizeAngle( float Degrees );

	//* Fast pow to ten
	inline float Pow10( int number );

	//* Error consideration
	inline float Atan2( float y, float x );
}

#include "Math/MathUtils.inl"
