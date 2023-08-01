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

    template<>
    struct MathConstants<double>
    {
        static constexpr double Pi = 3.1415926535897932384626433832795;
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
}

namespace EE::Math
{
    template <typename T> struct TVector2;
    template <typename T> struct TVector3;
    template <typename T> struct TVector4;
    template <typename T> struct TQuaternion;
    template <typename T> struct TMatrix4x4;
    template <typename T> struct TMatrix3x3;
    template <typename T> struct TPlane;
    template <typename T> struct TFrustrum;
    template <typename T> struct TBox3;
    template <typename T> struct TBox2;
    template <typename T> struct TIntBox2;
    template <typename T> struct TIntBox3;
    template <typename T> struct TIntVector2;
    template <typename T> struct TIntVector3;
    template <typename T> struct TIntVector4;

    template <typename T> using TPoint2 = TVector2<T>;
    template <typename T> using TPoint3 = TVector3<T>;
    template <typename T> using TPoint4 = TVector4<T>;
    template <typename T> using TIntPoint2 = TIntVector2<T>;
    template <typename T> using TIntPoint3 = TIntVector3<T>;
    template <typename T> using TIntPoint4 = TIntVector4<T>;

    template <typename T> using TBoundingBox2 = TBox2<T>;
    template <typename T> using TRect = TBox2<T>;
    template <typename T> using TBoundingBox3 = TBox3<T>;
    template <typename T> using TIntBoundingBox2 = TIntBox2<T>;
    template <typename T> using TIntRect = TIntBox2<T>;
    template <typename T> using TIntBoundingBox3 = TIntBox3<T>;

    template <typename T> class TTransform;
}

namespace EE
{
    using Vector2 = Math::TVector2<double>;
    using Vector3 = Math::TVector3<double>;
    using Vector4 = Math::TVector4<double>;
    using Point2 = Math::TPoint2<double>;
    using Point3 = Math::TPoint3<double>;
    using Point4 = Math::TPoint4<double>;
    using Quaternion = Math::TQuaternion<double>;
    using Matrix4x4 = Math::TMatrix4x4<double>;
    using Matrix3x3 = Math::TMatrix3x3<double>;
    using Plane = Math::TPlane<double>;
    using Transform = Math::TTransform<double>;
    using Box3 = Math::TBox3<double>;
    using Box2 = Math::TBox2<double>;
    using Rect = Math::TRect<double>;
    using BoundingBox3 = Math::TBoundingBox3<double>;
    using BoundingBox2 = Math::TBoundingBox2<double>;
    using Frustrum = Math::TFrustrum<double>;

    using Vector2f = Math::TVector2<float>;
    using Vector3f = Math::TVector3<float>;
    using Vector4f = Math::TVector4<float>;
    using Point2f = Math::TPoint2<float>;
    using Point3f = Math::TPoint3<float>;
    using Point4f = Math::TPoint4<float>;
    using Quaternionf = Math::TQuaternion<float>;
    using Matrix4x4f = Math::TMatrix4x4<float>;
    using Matrix3x3f = Math::TMatrix3x3<float>;
    using Planef = Math::TPlane<float>;
    using Transformf = Math::TTransform<float>;
    using Box3f = Math::TBox3<float>;
    using Box2f = Math::TBox2<float>;
    using Rectf = Math::TRect<float>;
    using BoundingBox3f = Math::TBoundingBox3<float>;
    using BoundingBox2f = Math::TBoundingBox2<float>;
    using Frustrumf = Math::TFrustrum<float>;

    using IntBox2 = Math::TIntBox2<int32>;
    using IntBox3 = Math::TIntBox3<int32>;
    using IntVector2 = Math::TIntVector2<int32>;
    using IntVector3 = Math::TIntVector3<int32>;
}

namespace EE::Math
{
    template <typename T>
	inline int32 SolveQuadratic( T x[ 2 ], T a, T b, T c );

    template <typename T>
	inline int32 SolveCubicNormed( T* x, T a, T b, T c );

    template <typename T>
	inline int32 SolveCubic( T x[ 3 ], T a, T b, T c, T d );

	template <typename T>
	inline float Shoelace2( const T& a, const T& b );
}

namespace EE::Math
{
	//* Returns the smaller of the arguments.
	template <typename T>
	inline T Min( const T& a, const T& b );

	//* Returns the larger of the arguments.
	template <typename T>
	inline T Max( const T& a, const T& b );

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

	//* Remap the value to another range of values
	template <typename T>
	inline T Map( const T& value, const T& minA, const T& maxA, const T& minB, const T& maxB );

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
    R Pow10( T number );

	//* Error consideration
    template <typename T>
	inline T Atan2( T y, T x );

    template <typename T>
    inline T Hypotenuse( T x, T y );
}

#include "Math/MathUtils.inl"
