#pragma once

namespace EE
{
    template<unsigned Frac> using FixVector2 = Math::TVector2<fixed<Frac>>;
    template<unsigned Frac> using FixVector3 = Math::TVector3<fixed<Frac>>;
    template<unsigned Frac> using FixVector4 = Math::TVector4<fixed<Frac>>;
    template<unsigned Frac> using FixPoint2 = Math::TPoint2<fixed<Frac>>;
    template<unsigned Frac> using FixPoint3 = Math::TPoint3<fixed<Frac>>;
    template<unsigned Frac> using FixPoint4 = Math::TPoint4<fixed<Frac>>;
    template<unsigned Frac> using FixQuaternion = Math::TQuaternion<fixed<Frac>>;
}