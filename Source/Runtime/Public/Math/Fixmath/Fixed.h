#pragma once

// The maximum value of fixed
#define FIXED_MAX 0x7FFFFFFF
// The minimum value of fixed
#define FIXED_MIN 0x80000000 
// The value used to indicate overflows when EE_FIXMATH_NO_OVERFLOW is not specified
#define FIXED_OVERFLOW 0x80000000

//         I N T E G R A L           F R A C T I O N A L ( F = 8 )
// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1
#define FIXED_FRACTIONAL_BIT_MASK( F ) (~(uint32)0 >> (32 - F))
#define FIXED_INTEGER_BIT_MASK( F ) (0xFFFFFFFF ^ FIXED_FRACTIONAL_BIT_MASK( F ))

// fixed value of 1
#define FIXED_ONE( F ) (0x00000001 << F)
// fixed value of pi
#define FIXED_PI( F ) (0x3243f6a88 >> (32 - F))
// fixed_t value of pi/2
#define FIXED_HALF_PI( F ) (0x1921FB542 >> (32 - F))
// fixed value of PI/4
#define FIXED_QUARTER_PI( F ) (0xC90FDAA1 >> (32 - F))
// fixed value of 4/PI
#define FIXED_FOUR_DIV_PI( F ) (0x1921FB54 >> (32 - F))
// fixed value of -4/PI²
#define FIXED_NEG_FOUR_DIV_PI2( F ) (0x983F4278 >> (32 - F))
// fixed value of 3PI/4
#define FIXED_THREE_PI_DIV4( F ) (0x25B2F8FE >> (32 - F))
// fix16_t value of e
#define FIXED_EPSILON( F ) (0x2B7E1515 >> (32 - F))
// fixed value of 0.225
#define FIXED_X4CORRECTION_COMPONENT( F ) (0x399B >> F)

// Refs
// http://www.sunshine2k.de/articles/coding/fp/sunfp.html

namespace EE
{
    template<unsigned Frac>
    struct fixed
    {
        template<unsigned OtherFrac>
        friend struct fixed;

    private:
        union
        {
            // Raw int value representing fixed-point
            int32 xValue;
        };

    private:
        // this makes it simpler to create a fixed point object from
        // a native type without scaling
        struct RawValue {};

        // Integer to fixed-point conversion.
        explicit FORCEINLINE constexpr fixed( const int32& value, const RawValue& ) : xValue( value ) {}

    public:

        static constexpr uint8 Fractional = Frac;

        // Fixed-point from raw int32.
        static FORCEINLINE constexpr fixed FromRaw( const int32& value )
        {
            return fixed( value, RawValue() );
        }

        // Get raw int32.
        FORCEINLINE constexpr const int32& Raw() const { return xValue; }

        explicit FORCEINLINE constexpr fixed() : xValue( 0 ) {}

        // Integer to fixed-point conversion.
        explicit FORCEINLINE constexpr fixed( const int32& value ) : xValue( ((int32)(value) << (Frac)) ) {}

        // Fixed-point to integer conversion.
        explicit FORCEINLINE constexpr operator int32() const
        {
            return (this->xValue >> (Frac));
        }

        // Fixed-point to unsigned integer conversion.
        explicit FORCEINLINE constexpr operator uint32() const
        {
            return (this->xValue >> (Frac));
        }

        // Conversion of different Frac sizes
        template<unsigned OtherFrac>
        explicit FORCEINLINE constexpr operator fixed<OtherFrac>() const
        {
            return fixed<OtherFrac>::FromRaw( (OtherFrac > Frac) ? xValue << (OtherFrac - Frac) : xValue >> (Frac - OtherFrac) );
        }

        // Single precision float to fixed-point conversion.
        explicit FORCEINLINE constexpr fixed( const float& value ) : xValue( (int32)((float)(value) * (float)FIXED_ONE( Frac )
#ifndef EE_FIXMATH_NO_ROUNDING
            + ((float)(value) > 0 ? 0.5F : -0.5F))
#endif
        ) { }

        // Fixed-point to single precision float conversion.
        explicit FORCEINLINE constexpr operator float() const
        {
            return ((float)(xValue) * 1.F / (float)FIXED_ONE( Frac ));
        }

        // Double precision float to fixed-point conversion.
        explicit FORCEINLINE constexpr fixed( const double& value ) : xValue( (int32)((double)(value) * (double)FIXED_ONE( Frac )
#ifndef EE_FIXMATH_NO_ROUNDING
            + ((double)(value) > 0 ? 0.5 : -0.5))
#endif
        ) { }

        // Fixed-point to double precision float conversion.
        explicit FORCEINLINE constexpr operator double() const
        {
            return ((double)(xValue) * 1.0 / (double)FIXED_ONE( Frac ));
        }

        static FORCEINLINE constexpr fixed Floor( const fixed& value )
        {
            return FromRaw( value.xValue & FIXED_INTEGER_BIT_MASK( Frac ) );
        }

        static FORCEINLINE constexpr fixed Round( const fixed& value )
        {
            if ( value.xValue == FIXED_MAX )
                return FromRaw( FIXED_MAX );

            return FromRaw( (value.xValue & FIXED_INTEGER_BIT_MASK( Frac )) + ((value.xValue & FIXED_FRACTIONAL_BIT_MASK( Frac )) >= (FIXED_FRACTIONAL_BIT_MASK( Frac ) / 2) ? FIXED_ONE( Frac ) : 0) );
        }

        static FORCEINLINE constexpr fixed Ceil( const fixed& value )
        {
            if ( value.xValue == FIXED_MAX )
                return FromRaw( FIXED_MAX );

            return FromRaw( (value.xValue & FIXED_INTEGER_BIT_MASK( Frac )) + ((value.xValue & FIXED_FRACTIONAL_BIT_MASK( Frac )) ? FIXED_ONE( Frac ) : 0) );
        }

        static FORCEINLINE constexpr fixed Abs( const fixed& value )
        {
            // Make a mask of the sign bit
            uint32 temp = value.xValue >> 31;
            fixed result = value;
            // Toggle the bits if value is negative
            result.xValue ^= temp;
            // Add one if value was negative
            result.xValue += temp & 1;
            return result;
        }

        static FORCEINLINE constexpr fixed Min( const fixed& x, const fixed& y )
        {
            return FromRaw( y.xValue ^ ((x.xValue ^ y.xValue) & -(x.xValue < y.xValue)) );
        }

        static FORCEINLINE constexpr fixed Max( const fixed& x, const fixed& y )
        {
            return FromRaw( x.xValue ^ ((x.xValue ^ y.xValue) & -(x.xValue < y.xValue)) );
        }

        static FORCEINLINE constexpr fixed Clamp( const fixed& value, const fixed& min, const fixed& max )
        {
            return Min( Max( value, min ), max );
        }

        // Addition arithmetic.
        static FORCEINLINE constexpr fixed FastAdd( const fixed& a, const fixed& b ) noexcept
        {
            fixed result;
            // Use unsigned integers because overflow with signed integers is
            // an undefined operation (http://www.airs.com/blog/archives/120).
            uint32 sum = (uint32)a.xValue + (uint32)b.xValue;
            result.xValue = sum;
            return result;
        }

        // Subtraction arithmetic.
        static FORCEINLINE constexpr fixed FastSubtract( const fixed& a, const fixed& b ) noexcept
        {
            fixed result;
            uint32_t diff = (uint32)a.xValue - (uint32)b.xValue;
            result.xValue = diff;
            return result;
        }

        // Saturating add arithmetic.
        static FORCEINLINE constexpr fixed Add( const fixed& a, const fixed& b ) noexcept
        {
            fixed result = FastAdd( a, b );

            // Overflow can only happen if sign of a == sign of b, and then
            // it causes sign of sum != sign of a.
            if ( (b.xValue == FIXED_OVERFLOW) || !((a.xValue ^ b.xValue) & FIXED_MIN) && ((a.xValue ^ result.xValue) & FIXED_MIN) )
            {
                result.xValue = (a.xValue > 0) ? FIXED_MAX : FIXED_MIN;
            }

            return result;
        }

        // Saturating sub arithmetic.
        static FORCEINLINE constexpr fixed Subtract( const fixed& a, const fixed& b ) noexcept
        {
            fixed result = FastSubtract( a, b );

            // Overflow can only happen if sign of a != sign of b, and then
            // it causes sign of diff != sign of a.
            if ( (b.xValue == FIXED_OVERFLOW) || ((a.xValue ^ b.xValue) & FIXED_MIN) && ((a.xValue ^ result.xValue) & FIXED_MIN) )
            {
                result.xValue = (a.xValue > 0) ? FIXED_MAX : FIXED_MIN;
            }

            return result;
        }

        /* 64-bit implementation for fix16_mul. Fastest version for e.g. ARM Cortex M3.
        * Performs a 32*32 -> 64bit multiplication. The middle 32 bits are the result,
        * bottom 16 bits are used for rounding, and upper integral + 1 bits are used for overflow
        * detection.
        */
        static FORCEINLINE constexpr fixed FastMultiply( const fixed& a, const fixed& b ) noexcept
        {
            int64 product = (int64)(a.xValue) * (int64)(b.xValue);

#ifdef EE_FIXMATH_NO_ROUNDING
#pragma warning( suppress : 4244 )
            fixed result; result.xValue = product >> (Frac - 1);
            return result;
#else
#pragma warning( suppress : 4244 )
            fixed result; result.xValue = product >> Frac;
            result.xValue += (product & (FIXED_MIN >> Frac )) >> (32 - Frac - 1);
            return result;
#endif
        }

        /* Like fast multiply but add saturating arithmetic.
        // Somehow the function above is faster than FastMultiply
        */
        static FORCEINLINE constexpr fixed Multiply( const fixed& a, const fixed& b ) noexcept
        {
            int64 product = (int64)(a.xValue) * (int64)(b.xValue);

            // The upper integral + 1 bits should all be the same (the sign).
            uint32 upper = (uint32)(product >> (32 + Frac - 1));

            if ( product < 0 )
            {
                if ( ~upper )
                {
                    fixed result;
                    result.xValue = FIXED_MIN;
                    return result;
                }

                // This adjustment is required in order to round -1/2 correctly
                product--;
            }
            else
            {
                if ( upper )
                {
                    fixed result;
                    result.xValue = FIXED_MAX;
                    return result;
                }
            }

#ifdef EE_FIXMATH_NO_ROUNDING
#pragma warning( suppress : 4244 )
            fixed result; result.xValue = product >> (Frac - 1);
            return result;
#else
#pragma warning( suppress : 4244 )
            fixed result; result.xValue = product >> Frac;
            result.xValue += (product & (FIXED_MIN >> Frac)) >> (32 - Frac - 1);
            return result;
#endif
        }

        /* 32-bit implementation of fix16_div. Fastest version for e.g. ARM Cortex M3.
         * Performs 32-bit divisions repeatedly to reduce the remainder. For this to
         * be efficient, the processor has to have 32-bit hardware division.
         */
#ifdef __GNUC__
     // Count leading zeros, using processor-specific instruction if available.
#define FIXED_CLZ(x) __builtin_clz(x)
#elif defined _MSC_VER
#define FIXED_CLZ(x) __lzcnt(x)
#else
        static constexpr uint8 clz( uint32 x )
        {
            uint8 result = 0;
            if ( x == 0 ) return 32;
            while ( !(x & 0xF0000000) ) { result += 4; x <<= 4; }
            while ( !(x & 0x80000000) ) { result += 1; x <<= 1; }
            return result;
        }
#endif

        // Straight forward division
        static FORCEINLINE constexpr fixed Divide( const fixed& a, const fixed& b )
        {
            if ( b.xValue == 0 )
            {
                return FromRaw( FIXED_MIN );
            }

            int64 remainder = (int64)a.xValue * (1 << Frac);
            int64 quotient = (int64)(remainder / b.xValue);

            if ( quotient > 0 && quotient & ~(FIXED_MAX) )
            {
                return FromRaw( FIXED_MAX );
            }

            if ( quotient < 0 && quotient <= 0xFFFFFFFF80000000 )
            {
                return FromRaw( FIXED_MIN );
            }

            return FromRaw( (int32)quotient );
        }

        //         // Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
        //         static EE_DLLEXPORT fixed Lerp8( const fixed& a, const fixed& b, const uint8& inFract ) noexcept;
        // 
        //         // Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
        //         static EE_DLLEXPORT fixed Lerp16( const fixed& a, const fixed& b, const uint16& inFract ) noexcept;
        // 
        // #ifndef EE_FIXMATH_NO_64BIT
        //         // Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
        //         static EE_DLLEXPORT fixed Lerp32( const fixed& a, const fixed& b, const uint32& inFract ) noexcept;
        // #endif
        // 
        //         // Returns the sine of the given fixed
        //         static EE_DLLEXPORT fixed SinParabola( const fixed& inAngle ) noexcept;

        // Returns the sine of the given fixed
        static FORCEINLINE constexpr fixed Sin( const fixed& radians ) noexcept
        {
            using fx28 = std::conditional_t<(28 < Frac), fixed<28>, fixed>;
            using fx25 = std::conditional_t<(25 < Frac), fixed<25>, fixed>;
            using fx22 = std::conditional_t<(22 < Frac), fixed<22>, fixed>;
            using fx19 = std::conditional_t<(19 < Frac), fixed<19>, fixed>;
            using fx16 = std::conditional_t<(16 < Frac), fixed<16>, fixed>;
            using fx13 = std::conditional_t<(13 < Frac), fixed<13>, fixed>;

            using fxSafe = std::conditional_t<(18 > Frac), fixed<18>, fixed>;

            fxSafe tempAngle = fxSafe::FromRaw( fxSafe(radians).xValue % (int32)FIXED_PI(fxSafe::Fractional - 1));
            if ( tempAngle.xValue > FIXED_PI( fxSafe::Fractional ) )
                tempAngle.xValue -= (int32)FIXED_PI( fxSafe::Fractional - 1);
            else if ( tempAngle.xValue < -FIXED_PI( fxSafe::Fractional ) )
                tempAngle.xValue += (int32)FIXED_PI( fxSafe::Fractional - 1);

            fixed tempOut = fixed(tempAngle);
            fx28 tempAngleSq = Math::Square( fx28( tempAngle ) );
            // Power 3 / 3!
            fx25 tempAngle_ = fx25::Multiply( fx25( tempAngle ), fx25( tempAngleSq ) );
            tempOut.xValue -= fixed( fx25::FromRaw( tempAngle_.Raw() / 6 ) ).Raw();
            // Power 5 / 5!
            fx22 tempAngle__ = fx22::Multiply( fx22( tempAngle_ ), fx22( tempAngleSq ) );
            tempOut.xValue += fixed( fx22::FromRaw( tempAngle__.Raw() / 120 ) ).Raw();
            // Power 7 / 7!
            fx19 tempAngle___ = fx19::Multiply( fx19( tempAngle__ ), fx19( tempAngleSq ) );
            tempOut.xValue -= fixed( fx19::FromRaw( tempAngle___.Raw() / 5040 ) ).Raw();
            // Power 9 / 9!
            fx16 tempAngle____ = fx16::Multiply( fx16( tempAngle___ ), fx16( tempAngleSq ) );
            tempOut.xValue += fixed( fx16::FromRaw( tempAngle____.Raw() / 362880 ) ).Raw();
            // Power 11 / 11!
            int32 tempAngleXValue = fx13::Multiply( fx13( tempAngle____ ), fx13( tempAngleSq ) ).Raw();
            tempOut.xValue -= fixed( fx13::FromRaw( tempAngleXValue / 39916800 ) ).Raw();

            return (fixed)tempOut;
        }

        // Returns the cosine of the given fixed
        static FORCEINLINE constexpr fixed Cos( const fixed& radians ) noexcept
        {
            using fx28 = std::conditional_t<(28 < Frac), fixed<28>, fixed>;
            using fx25 = std::conditional_t<(25 < Frac), fixed<25>, fixed>;
            using fx22 = std::conditional_t<(22 < Frac), fixed<22>, fixed>;
            using fx19 = std::conditional_t<(19 < Frac), fixed<19>, fixed>;
            using fx16 = std::conditional_t<(16 < Frac), fixed<16>, fixed>;
            using fx13 = std::conditional_t<(13 < Frac), fixed<13>, fixed>;

            using fxSafe = std::conditional_t<(18 > Frac), fixed<18>, fixed>;

            fxSafe tempAngle = fxSafe::FromRaw( fxSafe( radians ).xValue % (int32)FIXED_PI( fxSafe::Fractional - 1 ) );
            if ( tempAngle.xValue > FIXED_PI( fxSafe::Fractional ) )
                tempAngle.xValue -= (int32)FIXED_PI( fxSafe::Fractional - 1 );
            else if ( tempAngle.xValue < -FIXED_PI( fxSafe::Fractional ) )
                tempAngle.xValue += (int32)FIXED_PI( fxSafe::Fractional - 1 );

            fixed tempOut = fixed::FromRaw( FIXED_ONE( Frac ) );
            fx28 tempAngleSq = Math::Square( fx28( tempAngle ) );
            // Power 3 / 3!
            fx25 tempAngle_ = fx25::Multiply( fx25( tempOut ), fx25( tempAngleSq ) );
            tempOut.xValue -= fixed( fx25::FromRaw( tempAngle_.Raw() / 2 ) ).Raw();
            // Power 5 / 5!
            fx22 tempAngle__ = fx22::Multiply( fx22( tempAngle_ ), fx22( tempAngleSq ) );
            tempOut.xValue += fixed( fx22::FromRaw( tempAngle__.Raw() / 24 ) ).Raw();
            // Power 7 / 7!
            fx19 tempAngle___ = fx19::Multiply( fx19( tempAngle__ ), fx19( tempAngleSq ) );
            tempOut.xValue -= fixed( fx19::FromRaw( tempAngle___.Raw() / 720 ) ).Raw();
            // Power 9 / 9!
            fx16 tempAngle____ = fx16::Multiply( fx16( tempAngle___ ), fx16( tempAngleSq ) );
            tempOut.xValue += fixed( fx16::FromRaw( tempAngle____.Raw() / 40320 ) ).Raw();
            // Power 11 / 11!
            int32 tempAngleXValue = fx13::Multiply( fx13( tempAngle____ ), fx13( tempAngleSq ) ).Raw();
            tempOut.xValue -= fixed( fx13::FromRaw( tempAngleXValue / 3628800 ) ).Raw();

            return (fixed)tempOut;
        }

        // Returns the tangent of the given fixed
        static FORCEINLINE constexpr fixed Tan( const fixed& radians ) noexcept
        {
            return fixed::Sin( radians ) / fixed::Cos( radians );
        }
        
        static FORCEINLINE constexpr fixed ATan2( const fixed& inX, const fixed& inY ) noexcept
        {
            fixed angle, r, r3;

            /* Absolute inY */
            int32 mask = (inY.xValue >> (sizeof( int32 ) * CHAR_BIT - 1));
            fixed abs_inY = FromRaw( (inY.xValue + mask) ^ mask );

            if ( inY.xValue == 0 )
            {
                if ( inX.xValue >= 0 )
                {
                    angle = FromRaw( 0 );
                }
                else
                {
                    angle = FromRaw( FIXED_HALF_PI( Frac ) );
                }
            }
            else if ( inX.xValue >= 0 )
            {
                r = (inX - abs_inY) / (inX + abs_inY);
                r3 = r * r * r;
                                       // 11.249                            56.24
                angle = FromRaw( 0xFB50FB50 >> (32 - Frac) ) * r - FromRaw( 0x3240323F >> (32 - Frac) ) * r3 + FromRaw( FIXED_QUARTER_PI( Frac ) );
            }
            else
            {
                r = (inX + abs_inY) / (abs_inY - inX);
                r3 = r * r * r;
                angle =  FromRaw( 0xFB50FB50 >> (32 - Frac) ) * r - FromRaw( 0x3240323F >> (32 - Frac) ) * r3 - FromRaw( FIXED_QUARTER_PI( Frac ) );
            }
            if ( inY.xValue < 0 )
            {
                // negate if in quad III or IV
                angle = FromRaw( -angle.xValue ); 
            }

            return angle;
        }

        static FORCEINLINE constexpr fixed ATan( const fixed& value ) noexcept
        {
            return ATan2( value, FromRaw( FIXED_ONE( Frac ) ) );
        }

        static FORCEINLINE constexpr fixed ASin( const fixed& value ) noexcept
        {
            if ( (value.xValue > FIXED_ONE( Frac )) || (value.xValue < -FIXED_ONE( Frac )) )
                return fixed();

            fixed tempOut;
            tempOut = FromRaw( FIXED_ONE( Frac ) ) - value * value;
            tempOut = value / fixed::Sqrt( tempOut );
            tempOut = fixed::ATan( tempOut );
            return tempOut;
        }

        // 
        //         // Returns the cosine of the given fixed
        //         static EE_DLLEXPORT fixed Cos( const fixed& inAngle ) noexcept;
        // 
        //         // Returns the tangent of the given fixed
        //         static EE_DLLEXPORT fixed Tan( const fixed& inAngle ) noexcept;
        // 
        //         // Returns the arcsine of the given fixed
        //         static EE_DLLEXPORT fixed ASin( const fixed& inValue ) noexcept;
        // 
        //         // Returns the arccosine of the given fixed
        //         static EE_DLLEXPORT fixed ACos( const fixed& inValue ) noexcept;
        // 
        //         // Returns the arctangent of the given fixed
        //         static EE_DLLEXPORT fixed ATan( const fixed& inValue ) noexcept;
        // 
        //         // Returns the arctangent of inY/inX
        //         static EE_DLLEXPORT fixed ATan2( const fixed& inY, const fixed& inX ) noexcept;
        //

        // The remainder of the division operation a / b
        static FORCEINLINE constexpr fixed Modulus( const fixed& a, const fixed& b ) noexcept
        {
            fixed result;
            uint32 modulus = (uint32)a.xValue % (uint32)b.xValue;
            result.xValue = modulus;

            if ( a.xValue < 0 || b.xValue < 0 )
                result.xValue -= b.xValue;

            return result;
        }

        // Returns the square root of the given fixed
        static FORCEINLINE constexpr fixed Sqrt( const fixed& value ) noexcept
        {
            fixed result = fixed( 1 );

            int iteration = 0;
            while ( (iteration++ < 10) && (Abs( (result * result) - value ).xValue >= 1) )
            {
                result = fixed( 0.5 ) * (result + (value / result));
            }

            return result;
        }
        // 
        //         // Returns the exponent (e^) of the given fixed
        //         static EE_DLLEXPORT fixed Exp( const fixed& inValue ) noexcept;
        // 
        FORCEINLINE fixed operator*( const fixed& other ) const
        {
            return Multiply( *this, other );
        }

        FORCEINLINE fixed& operator*=( const fixed& other )
        {
            xValue = Multiply( *this, other ).xValue;
            return *this;
        }
        
        FORCEINLINE fixed operator/( const fixed& other ) const
        {
            return Divide( *this, other );
        }
        
        FORCEINLINE fixed& operator/=( const fixed& other )
        {
            xValue = Divide( *this, other ).xValue;
            return *this;
        }
        
        FORCEINLINE constexpr fixed operator+( const fixed& other ) const
        {
            return Add( *this, other );
        }

        FORCEINLINE fixed& operator+=( const fixed& other )
        {
            xValue = Add( *this, other ).xValue;
            return *this;
        }

        FORCEINLINE constexpr fixed operator-() const
        {
            return FromRaw( -xValue );
        }

        FORCEINLINE constexpr fixed operator-( const fixed& other ) const
        {
            return Subtract( *this, other );
        }

        FORCEINLINE fixed& operator-=( const fixed& other )
        {
            xValue = Subtract( *this, other ).xValue;
            return *this;
        }

        FORCEINLINE fixed operator%( const fixed& other ) const
        {
            return Modulus( *this, other );
        }

        FORCEINLINE constexpr bool operator>( const fixed& other ) const
        {
            return this->xValue > other.xValue;
        }

        FORCEINLINE constexpr bool operator>=( const fixed& other ) const
        {
            return this->xValue >= other.xValue;
        }

        FORCEINLINE constexpr bool operator<( const fixed& other ) const
        {
            return this->xValue < other.xValue;
        }

        FORCEINLINE constexpr bool operator<=( const fixed& other ) const
        {
            return this->xValue < other.xValue;
        }

        FORCEINLINE constexpr bool operator==( const fixed& other ) const
        {
            return this->xValue == other.xValue;
        }
    };
}