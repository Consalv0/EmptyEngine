#pragma once

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

    template <typename T> using TRect = TBox2<T>;
    template <typename T> using TIntRect = TIntBox2<T>;

    template <typename T> class TTransform;
}

namespace EE
{
    using Vector2 = Math::TVector2<double>;
    using Vector3 = Math::TVector3<double>;
    using Vector4 = Math::TVector4<double>;
    using Quaternion = Math::TQuaternion<double>;
    using Matrix4x4 = Math::TMatrix4x4<double>;
    using Matrix3x3 = Math::TMatrix3x3<double>;
    using Plane = Math::TPlane<double>;
    using Transform = Math::TTransform<double>;
    using Box3 = Math::TBox3<double>;
    using Box2 = Math::TBox2<double>;
    using Rect = Math::TRect<double>;
    using Frustrum = Math::TFrustrum<double>;

    using Vector2f = Math::TVector2<float>;
    using Vector3f = Math::TVector3<float>;
    using Vector4f = Math::TVector4<float>;
    using Quaternionf = Math::TQuaternion<float>;
    using Matrix4x4f = Math::TMatrix4x4<float>;
    using Matrix3x3f = Math::TMatrix3x3<float>;
    using Planef = Math::TPlane<float>;
    using Transformf = Math::TTransform<float>;
    using Box3f = Math::TBox3<float>;
    using Box2f = Math::TBox2<float>;
    using Rectf = Math::TRect<float>;
    using Frustrumf = Math::TFrustrum<float>;

    using IntBox2 = Math::TIntBox2<int32>;
    using IntBox3 = Math::TIntBox3<int32>;
    using IntVector2 = Math::TIntVector2<int32>;
    using IntVector3 = Math::TIntVector3<int32>;
    using IntVector4 = Math::TIntVector4<int32>;

    using UIntVector2 = Math::TIntVector2<uint32>;
    using UIntVector3 = Math::TIntVector3<uint32>;
}

#define EE_CLAMP(x, lo, hi)    ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))