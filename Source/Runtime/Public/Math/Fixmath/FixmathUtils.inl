
namespace EE
{
    template<unsigned Frac>
    fixed<Frac> Math::Square( const fixed<Frac>& value )
    {
        return value * value;
    }

    template<unsigned Frac>
    fixed<Frac> Math::Sqrt( const fixed<Frac>& value )
    {
        return fixed<Frac>::Sqrt( value );
    }

    template<unsigned Frac>
    fixed<Frac> Math::Modulus( const fixed<Frac>& a, const fixed<Frac>& b )
    {
        return fixed<Frac>::Modulus( a, b );
    }

    template<unsigned Frac>
    fixed<Frac> Math::Sin( const fixed<Frac>& radians )
    {
        return fixed<Frac>::Sin( radians );
    }

    template<unsigned Frac>
    fixed<Frac> Math::Cos( const fixed<Frac>& radians )
    {
        return fixed<Frac>::Cos( radians );
    }

    template<unsigned Frac>
    fixed<Frac> Math::Tan( const fixed<Frac>& radians )
    {
        return fixed<Frac>::Tan( radians );
    }

    template<unsigned Frac>
    fixed<Frac> Math::ATan( const fixed<Frac>& value )
    {
        return fixed<Frac>::ATan( value );
    }

    template<unsigned Frac>
    fixed<Frac> Math::ATan2( const fixed<Frac>& x, const fixed<Frac>& y )
    {
        return fixed<Frac>::ATan2( x, y );
    }
}