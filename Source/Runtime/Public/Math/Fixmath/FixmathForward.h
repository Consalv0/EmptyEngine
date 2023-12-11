#pragma once

namespace EE
{
    template<unsigned Frac> using FixedVector2      = Math::TVector2      <fixed<Frac>>;
    template<unsigned Frac> using FixedVector3      = Math::TVector3      <fixed<Frac>>;
    template<unsigned Frac> using FixedVector4      = Math::TVector4      <fixed<Frac>>;
    template<unsigned Frac> using FixedPoint2       = Math::TPoint2       <fixed<Frac>>;
    template<unsigned Frac> using FixedPoint3       = Math::TPoint3       <fixed<Frac>>;
    template<unsigned Frac> using FixedPoint4       = Math::TPoint4       <fixed<Frac>>;
    template<unsigned Frac> using FixedQuaternion   = Math::TQuaternion   <fixed<Frac>>;
    template<unsigned Frac> using FixedMatrix4x4    = Math::TMatrix4x4    <fixed<Frac>>;
    template<unsigned Frac> using FixedMatrix3x3    = Math::TMatrix3x3    <fixed<Frac>>;
    template<unsigned Frac> using FixedPlane        = Math::TPlane        <fixed<Frac>>;
    template<unsigned Frac> using FixedTransform    = Math::TTransform    <fixed<Frac>>;
    template<unsigned Frac> using FixedBox3         = Math::TBox3         <fixed<Frac>>;
    template<unsigned Frac> using FixedBox2         = Math::TBox2         <fixed<Frac>>;
    template<unsigned Frac> using FixedRect         = Math::TRect         <fixed<Frac>>;
    template<unsigned Frac> using FixedBoundingBox3 = Math::TBoundingBox3 <fixed<Frac>>;
    template<unsigned Frac> using FixedBoundingBox2 = Math::TBoundingBox2 <fixed<Frac>>;
    template<unsigned Frac> using FixedFrustrum     = Math::TFrustrum     <fixed<Frac>>;
}

namespace EE
{
    using fix8 = fixed<18>;
    using Fix8Vector2           = FixedVector2       <18>;
    using Fix8Vector3           = FixedVector3       <18>;
    using Fix8Vector4           = FixedVector4       <18>;
    using Fix8Point2            = FixedPoint2        <18>;
    using Fix8Point3            = FixedPoint3        <18>;
    using Fix8Point4            = FixedPoint4        <18>;
    using Fix8Quaternion        = FixedQuaternion    <18>;
    using Fix8Matrix4x4         = FixedMatrix4x4     <18>;
    using Fix8Matrix3x3         = FixedMatrix3x3     <18>;
    using Fix8Plane             = FixedPlane         <18>;
    using Fix8Transform         = FixedTransform     <18>;
    using Fix8Box3              = FixedBox3          <18>;
    using Fix8Box2              = FixedBox2          <18>;
    using Fix8Rect              = FixedRect          <18>;
    using Fix8BoundingBox3      = FixedBoundingBox3  <18>;
    using Fix8BoundingBox2      = FixedBoundingBox2  <18>;
    using Fix8Frustrum          = FixedFrustrum      <18>;
}