
#include "CoreMinimal.h"

#include "Math/CoreMath.h"
#include "Math/Fixmath/Fixed.h"

namespace EE
{
//     /* Subtraction and addition with overflow detection.
//      * The versions without overflow detection are inlined in the header.
//      */
// #ifndef EE_FIXMATH_NO_OVERFLOW
// 
//     fixed fixed::Add( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result;
// 
//         // Use unsigned integers because overflow with signed integers is
//         // an undefined operation (http://www.airs.com/blog/archives/120).
//         uint32 _a = a.value, _b = b.value;
//         uint32 sum = _a + _b;
// 
//         // Overflow can only happen if sign of a == sign of b, and then
//         // it causes sign of sum != sign of a.
//         if ( !((_a ^ _b) & FIXED_MIN) && ((_a ^ sum) & FIXED_MIN) )
//         {
//             result.value = FIX16_OVERFLOW;
//             return result;
//         }
// 
//         result.value = sum;
//         return result;
//     }
// 
//     fixed fixed::Substract( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result;
//         uint32_t _a = a.value, _b = b.value;
//         uint32_t diff = _a - _b;
// 
//         // Overflow can only happen if sign of a != sign of b, and then
//         // it causes sign of diff != sign of a.
//         if ( ((_a ^ _b) & 0x80000000) && ((_a ^ diff) & 0x80000000) )
//         {
//             result.value = FIX16_OVERFLOW;
//             return result;
//         }
// 
//         result.value = diff;
//         return result;
//     }
// 
//     /* Saturating arithmetic */
//     fixed fixed::SaturateAdd( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result = Add( a, b );
// 
//         if ( result.value == FIX16_OVERFLOW )
//             return fix16_t( (a.value > 0) ? FIXED_MAX : FIXED_MIN );
// 
//         return result;
//     }
// 
//     fixed fixed::SaturateSub( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result = Substract( a, b );
// 
//         if ( result.value == FIX16_OVERFLOW )
//             return fix16_t( (a.value > 0) ? FIXED_MAX : FIXED_MIN );
// 
//         return result;
//     }
// #endif
// 
//     /* 64-bit implementation for fix16_mul. Fastest version for e.g. ARM Cortex M3.
//      * Performs a 32*32 -> 64bit multiplication. The middle 32 bits are the result,
//      * bottom 16 bits are used for rounding, and upper 16 bits are used for overflow
//      * detection.
//      */
// 
// #if !defined(EE_FIXMATH_NO_64BIT) && !defined(EE_FIXMATH_OPTIMIZE_8BIT)
//     fixed fixed::Multiply( const fixed& a, const fixed& b ) noexcept
//     {
//         int64 product = (int64)a.value * b.value;
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         // The upper 17 bits should all be the same (the sign).
//         uint32 upper = (product >> 47);
// #endif
// 
//         if ( product < 0 )
//         {
// #ifndef EE_FIXMATH_NO_OVERFLOW
//             if ( ~upper )
//             {
//                 return fix16_t( FIX16_OVERFLOW );
//             }
// #endif
// 
// #ifndef EE_FIXMATH_NO_ROUNDING
//             // This adjustment is required in order to round -1/2 correctly
//             product--;
// #endif
//         }
//         else
//         {
// #ifndef EE_FIXMATH_NO_OVERFLOW
//             if ( upper )
//             {
//                 return fix16_t( FIX16_OVERFLOW );
//             }
// #endif
//         }
// 
// #ifdef EE_FIXMATH_NO_ROUNDING
// #pragma warning( suppress : 4244 )
//         fix16_t result = product >> 16;
//         return result;
// #else
// #pragma warning( suppress : 4244 )
//         fix16_t result = product >> 16;
//         result.value += (product & 0x8000) >> 15;
// 
//         return result;
// #endif
//     }
// #endif
// 
//     /* 32-bit implementation of fix16_mul. Potentially fast on 16-bit processors,
//      * and this is a relatively good compromise for compilers that do not support
//      * uint64_t. Uses 16*16->32bit multiplications.
//      */
// #if defined(EE_FIXMATH_NO_64BIT) && !defined(EE_FIXMATH_OPTIMIZE_8BIT)
//     fixed fixed::Muliply( const fixed& a, const fixed& b ) noexcept
//     {
//         // Each argument is divided to 16-bit parts.
//         //          AB
//         //      *   CD
//         // -----------
//         //          BD  16 * 16 -> 32 bit products
//         //         CB
//         //         AD
//         //        AC
//         //       |----| 64 bit product
//         int32 A = (a.value >> 16), C = (b.value >> 16);
//         uint32 B = (a.value & 0xFFFF), D = (b.value & 0xFFFF);
// 
//         int32 AC = A * C;
//         int32 AD_CB = A * D + C * B;
//         uint32 BD = B * D;
// 
//         int32 product_hi = AC + (AD_CB >> 16);
// 
//         // Handle carry from lower 32 bits to upper part of result.
//         uint32 ad_cb_temp = AD_CB << 16;
//         uint32 product_lo = BD + ad_cb_temp;
//         if ( product_lo < BD )
//             product_hi++;
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         // The upper 17 bits should all be the same (the sign).
//         if ( product_hi >> 31 != product_hi >> 15 )
//             return fix16_t( FIX16_OVERFLOW );
// #endif
// 
// #ifdef EE_FIXMATH_NO_ROUNDING
//         return (product_hi << 16) | (product_lo >> 16);
// #else
//         // Subtracting 0x8000 (= 0.5) and then using signed right shift
//         // achieves proper rounding to result-1, except in the corner
//         // case of negative numbers and lowest word = 0x8000.
//         // To handle that, we also have to subtract 1 for negative numbers.
//         uint32 product_lo_tmp = product_lo;
//         product_lo -= 0x8000;
//         product_lo -= (uint32)product_hi >> 31;
//         if ( product_lo > product_lo_tmp )
//             product_hi--;
// 
//         // Discard the lowest 16 bits. Note that this is not exactly the same
//         // as dividing by 0x10000. For example if product = -1, result will
//         // also be -1 and not 0. This is compensated by adding +1 to the result
//         // and compensating this in turn in the rounding above.
//         int32 result = (product_hi << 16) | (product_lo >> 16);
//         result += 1;
//         return fix16_t( result );
// #endif
//     }
// #endif
// 
//     /* 8-bit implementation of fix16_mul. Fastest on e.g. Atmel AVR.
//      * Uses 8*8->16bit multiplications, and also skips any bytes that
//      * are zero.
//      */
// #if defined(EE_FIXMATH_OPTIMIZE_8BIT)
//     fixed Multiply( const fixed& a, const fixed& b ) noexcept
//     {
//         uint32 _a = (a.value >= 0) ? a.value : (-a.value);
//         uint32 _b = (b.value >= 0) ? b.value : (-b.value);
// 
//         uint8 va[ 4 ] = { _a, (_a >> 8), (_a >> 16), (_a >> 24) };
//         uint8 vb[ 4 ] = { _b, (_b >> 8), (_b >> 16), (_b >> 24) };
// 
//         uint32 low = 0;
//         uint32 mid = 0;
// 
//         // Result column i depends on va[0..i] and vb[i..0]
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
// // i = 6
//         if ( va[ 3 ] && vb[ 3 ] )
//         {
//             return fix16_t( FIX16_OVERFLOW );
//         }
// #endif
// 
//         // i = 5
//         if ( va[ 2 ] && vb[ 3 ] ) mid += (uint16)va[ 2 ] * vb[ 3 ];
//         if ( va[ 3 ] && vb[ 2 ] ) mid += (uint16)va[ 3 ] * vb[ 2 ];
//         mid <<= 8;
// 
//         // i = 4
//         if ( va[ 1 ] && vb[ 3 ] ) mid += (uint16)va[ 1 ] * vb[ 3 ];
//         if ( va[ 2 ] && vb[ 2 ] ) mid += (uint16)va[ 2 ] * vb[ 2 ];
//         if ( va[ 3 ] && vb[ 1 ] ) mid += (uint16)va[ 3 ] * vb[ 1 ];
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         if ( mid & 0xFF000000 )
//         {
//             return fix16_t( FIX16_OVERFLOW );
//         }
// #endif
//         mid <<= 8;
// 
//         // i = 3
//         if ( va[ 0 ] && vb[ 3 ] ) mid += (uint16)va[ 0 ] * vb[ 3 ];
//         if ( va[ 1 ] && vb[ 2 ] ) mid += (uint16)va[ 1 ] * vb[ 2 ];
//         if ( va[ 2 ] && vb[ 1 ] ) mid += (uint16)va[ 2 ] * vb[ 1 ];
//         if ( va[ 3 ] && vb[ 0 ] ) mid += (uint16)va[ 3 ] * vb[ 0 ];
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         if ( mid & 0xFF000000 )
//         {
//              return fix16_t( FIX16_OVERFLOW );
//         }
// #endif
//         mid <<= 8;
// 
//         // i = 2
//         if ( va[ 0 ] && vb[ 2 ] ) mid += (uint16)va[ 0 ] * vb[ 2 ];
//         if ( va[ 1 ] && vb[ 1 ] ) mid += (uint16)va[ 1 ] * vb[ 1 ];
//         if ( va[ 2 ] && vb[ 0 ] ) mid += (uint16)va[ 2 ] * vb[ 0 ];
// 
//         // i = 1
//         if ( va[ 0 ] && vb[ 1 ] ) low += (uint16)va[ 0 ] * vb[ 1 ];
//         if ( va[ 1 ] && vb[ 0 ] ) low += (uint16)va[ 1 ] * vb[ 0 ];
//         low <<= 8;
// 
//         // i = 0
//         if ( va[ 0 ] && vb[ 0 ] ) low += (uint16)va[ 0 ] * vb[ 0 ];
// 
// #ifndef EE_FIXMATH_NO_ROUNDING
//         low += 0x8000;
// #endif
//         mid += (low >> 16);
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         if ( mid & 0x80000000 )
//         {
//             return fix16_t( FIX16_OVERFLOW );
//         }
// #endif
// 
//         int32 result = mid;
// 
//         /* Figure out the sign of result */
//         if ( (a.value >= 0) != (b.value >= 0) )
//         {
//             result = -result;
//         }
// 
//         return fix16_t( result );
//     }
// #endif
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//     /* Wrapper around fix16_mul to add saturating arithmetic. */
//     fixed fixed::SaturateMultiply( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result = Multiply( a, b );
// 
//         if ( result.value == FIX16_OVERFLOW )
//         {
//             if ( (a.value >= 0) == (b.value >= 0) )
//                 return fix16_t( FIXED_MAX );
//             else
//                 return fix16_t( FIXED_MIN );
//         }
// 
//         return result;
//     }
// #endif
// 
//     /* 32-bit implementation of fix16_div. Fastest version for e.g. ARM Cortex M3.
//      * Performs 32-bit divisions repeatedly to reduce the remainder. For this to
//      * be efficient, the processor has to have 32-bit hardware division.
//      */
// #if !defined(EE_FIXMATH_OPTIMIZE_8BIT)
// #ifdef __GNUC__
//      // Count leading zeros, using processor-specific instruction if available.
// #define clz(x) __builtin_clzl(x)
// #else
//     static constexpr uint8 clz( uint32 x )
//     {
//         uint8 result = 0;
//         if ( x == 0 ) return 32;
//         while ( !(x & 0xF0000000) ) { result += 4; x <<= 4; }
//         while ( !(x & 0x80000000) ) { result += 1; x <<= 1; }
//         return result;
//     }
// #endif
// 
//     fixed fixed::Divide( const fixed& a, const fixed& b ) noexcept
//     {
//         // This uses a hardware 32/32 bit division multiple times, until we have
//         // computed all the bits in (a<<17)/b. Usually this takes 1-3 iterations.
// 
//         if ( b.value == 0 )
//         {
//             return fix16_t( FIXED_MIN );
//         }
// 
//         uint32 remainder = (a.value >= 0) ? a.value : (-a.value);
//         uint32 divider = (b.value >= 0) ? b.value : (-b.value);
//         uint32 quotient = 0;
//         int bit_pos = 17;
// 
//         // Kick-start the division a bit.
//         // This improves speed in the worst-case scenarios where N and D are large
//         // It gets a lower estimate for the result by N/(D >> 17 + 1).
//         if ( divider & 0xFFF00000 )
//         {
//             uint32 shifted_div = ((divider >> 17) + 1);
//             quotient = remainder / shifted_div;
//             remainder -= ((uint64)quotient * divider) >> 17;
//         }
// 
//         // If the divider is divisible by 2^n, take advantage of it.
//         while ( !(divider & 0xF) && bit_pos >= 4 )
//         {
//             divider >>= 4;
//             bit_pos -= 4;
//         }
// 
//         while ( remainder && bit_pos >= 0 )
//         {
//             // Shift remainder as much as we can without overflowing
//             int shift = clz( remainder );
//             if ( shift > bit_pos ) shift = bit_pos;
//             remainder <<= shift;
//             bit_pos -= shift;
// 
//             uint32 div = remainder / divider;
//             remainder = remainder % divider;
//             quotient += div << bit_pos;
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//             if ( div & ~(0xFFFFFFFF >> bit_pos) )
//             {
//                 return fix16_t( FIX16_OVERFLOW );
//             }
// #endif
// 
//             remainder <<= 1;
//             bit_pos--;
//         }
// 
// #ifndef EE_FIXMATH_NO_ROUNDING
//         // Quotient is always positive so rounding is easy
//         quotient++;
// #endif
// 
//         int32 result = quotient >> 1;
// 
//         // Figure out the sign of the result
//         if ( (a.value ^ b.value) & 0x80000000 )
//         {
// #ifndef EE_FIXMATH_NO_OVERFLOW
//             if ( result == FIXED_MIN )
//             {
//                 return fix16_t( FIX16_OVERFLOW );
//             }
// #endif
// 
//             result = -result;
//         }
// 
//         return fix16_t( result );
//     }
// #endif
// 
//     /* Alternative 32-bit implementation of fix16_div. Fastest on e.g. Atmel AVR.
//      * This does the division manually, and is therefore good for processors that
//      * do not have hardware division.
//      */
// #if defined(EE_FIXMATH_OPTIMIZE_8BIT)
//     fixed fixed::Divide( const fixed& a, const fixed& b ) noexcept
//     {
//         // This uses the basic binary restoring division algorithm.
//         // It appears to be faster to do the whole division manually than
//         // trying to compose a 64-bit divide out of 32-bit divisions on
//         // platforms without hardware divide.
// 
//         if ( b.value == 0 )
//         {
//             value = FIXED_MIN;
//             return;
//         }
// 
//         uint32 remainder = (a.value >= 0) ? a.value : (-a.value);
//         uint32 divider = (b.value >= 0) ? b.value : (-b.value);
// 
//         uint32 quotient = 0;
//         uint32 bit = 0x10000;
// 
//         /* The algorithm requires D >= R */
//         while ( divider < remainder )
//         {
//             divider <<= 1;
//             bit <<= 1;
//         }
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         if ( !bit )
//         {
//             return fix16_t( FIX16_OVERFLOW );
//         }
// #endif
// 
//         if ( divider & 0x80000000 )
//         {
//             // Perform one step manually to avoid overflows later.
//             // We know that divider's bottom bit is 0 here.
//             if ( remainder >= divider )
//             {
//                 quotient |= bit;
//                 remainder -= divider;
//             }
//             divider >>= 1;
//             bit >>= 1;
//         }
// 
//         /* Main division loop */
//         while ( bit && remainder )
//         {
//             if ( remainder >= divider )
//             {
//                 quotient |= bit;
//                 remainder -= divider;
//             }
// 
//             remainder <<= 1;
//             bit >>= 1;
//         }
// 
// #ifndef EE_FIXMATH_NO_ROUNDING
//         if ( remainder >= divider )
//         {
//             quotient++;
//         }
// #endif
// 
//         int32 result = quotient;
// 
//         /* Figure out the sign of result */
//         if ( (a.value ^ b.value) & 0x80000000 )
//         {
// #ifndef EE_FIXMATH_NO_OVERFLOW
//             if ( result == FIXED_MIN )
//             {
//                 return fix16_t( FIX16_OVERFLOW );
//             }
// #endif
// 
//             result = -result;
//         }
// 
//         return fix16_t( result );
//     }
// #endif
// 
// #ifndef EE_FIXMATH_NO_OVERFLOW
//     /* Wrapper around fix16_div to add saturating arithmetic. */
//     fixed fixed::SaturateDivide( const fixed& a, const fixed& b ) noexcept
//     {
//         fixed result = Divide( a, b );
// 
//         if ( result.value == FIX16_OVERFLOW )
//         {
//             if ( (a.value >= 0) == (b.value >= 0) )
//                 return fix16_t( FIXED_MAX );
//             else
//                 return fix16_t( FIXED_MIN );
//         }
// 
//         return result;
//     }
// #endif
// 
//     fixed fixed::Lerp8( const fixed& a, const fixed& b, const uint8& inFract ) noexcept
//     {
//         int64 tempOut = a.value * ((1 << 8) - inFract);
//         tempOut = tempOut + (b.value * inFract);
//         tempOut = (tempOut >> 8);
//         return fix16_t( (int32)(tempOut & ((1ULL << 32) - 1)) );
//     }
// 
//     fixed fixed::Lerp16( const fixed& a, const fixed& b, const uint16& inFract ) noexcept
//     {
//         int64 tempOut = a.value * ((1 << 16) - inFract);
//         tempOut = tempOut + (b.value * inFract);
//         tempOut = (tempOut >> 16);
//         return fix16_t( (int32)(tempOut & ((1ULL << 32) - 1)) );
//     }
// 
// #ifndef EE_FIXMATH_NO_64BIT
//     fixed fixed::Lerp32( const fixed& a, const fixed& b, const uint32& inFract ) noexcept
//     {
//         int64 tempOut;
//         tempOut = ((int64)a.value * (0 - inFract));
//         tempOut += ((int64)b.value * inFract);
//         tempOut >>= 32;
//         return fix16_t( (int32)tempOut );
//     }
// #endif
// 
// #if defined(EE_FIXMATH_SIN_LUT)
// #include "Math/Fixmath/Fix16_SinLut.h"
// #elif !defined(EE_FIXMATH_NO_CACHE)
//     static int32 _fix16_sin_cache_index[ 4096 ] = { 0 };
//     static int32 _fix16_sin_cache_value[ 4096 ] = { 0 };
// #endif
// 
// #ifndef EE_FIXMATH_NO_CACHE
//     static int32 _fix16_atan_cache_index[ 2 ][ 4096 ] = { { 0 }, { 0 } };
//     static int32 _fix16_atan_cache_value[ 4096 ] = { 0 };
// #endif
// 
//     fixed fixed::SinParabola( const fixed& inAngle ) noexcept
//     {
//         int32 abs_inAngle, abs_retval, retval;
//         int32 mask;
// 
//         /* Absolute function */
//         mask = (inAngle.value >> (sizeof( int32 ) * CHAR_BIT - 1));
//         abs_inAngle = (inAngle.value + mask) ^ mask;
// 
//         /* On 0->PI, sin looks like x² that is :
//            - centered on PI/2,
//            - equals 1 on PI/2,
//            - equals 0 on 0 and PI
//           that means :  4/PI * x  - 4/PI² * x²
//           Use abs(x) to handle (-PI) -> 0 zone.
//          */
//         fixed a = Multiply( fix16_t( FIX16_FOUR_DIV_PI ), inAngle );
//         fixed b = Multiply( fix16_t( FIX16_NEG_FOUR_DIV_PI2 ), inAngle );
//         fixed c = Multiply( b, fix16_t( abs_inAngle ) );
//         fixed d = Multiply( b, c );
// 
//         retval = a.value + d.value;
//         /* At this point, retval equals sin(inAngle) on important points ( -PI, -PI/2, 0, PI/2, PI),
//            but is not very precise between these points
//          */
// #ifndef EE_FIXMATH_FAST_SIN
//          /* Absolute value of retval */
//         mask = (retval >> (sizeof( int32 ) * CHAR_BIT - 1));
//         abs_retval = (retval + mask) ^ mask;
//         /* So improve its precision by adding some x^4 component to retval */
//         fixed retvalMulAbs = Multiply( fix16_t( retval ), fix16_t( abs_retval ) );
//         fixed x = Multiply( fix16_t( FIX16_X4CORRECTION_COMPONENT ), fix16_t( retvalMulAbs.value - retval ) );
//         retval += x.value;
// #endif
//         return fix16_t( retval );
//     }
// 
//     fixed fixed::Sin( const fixed& inAngle ) noexcept
//     {
//         int32 tempAngle = inAngle.value % (FIX16_PI << 1);
// 
// #ifdef EE_FIXMATH_SIN_LUT
//         if ( tempAngle < 0 )
//             tempAngle += (FIX16_PI << 1);
// 
//         fixed tempOut;
//         if ( tempAngle >= FIX16_PI )
//         {
//             tempAngle -= FIX16_PI;
//             if ( tempAngle >= (FIX16_PI >> 1) )
//                 tempAngle = FIX16_PI - tempAngle;
//             tempOut = -(tempAngle >= _fix16_sin_lut_count ? FIX16_ONE : _fix16_sin_lut[ tempAngle ]);
//         }
//         else
//         {
//             if ( tempAngle >= (FIX16_PI >> 1) )
//                 tempAngle = FIX16_PI - tempAngle;
//             tempOut = (tempAngle >= _fix16_sin_lut_count ? FIX16_ONE : _fix16_sin_lut[ tempAngle ]);
//         }
// #else
//         if ( tempAngle > FIX16_PI )
//             tempAngle -= (FIX16_PI << 1);
//         else if ( tempAngle < -FIX16_PI )
//             tempAngle += (FIX16_PI << 1);
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         int32 tempIndex = ((inAngle.value >> 5) & 0x00000FFF);
//         if ( _fix16_sin_cache_index[ tempIndex ] == inAngle.value )
//             return (fix16_t)_fix16_sin_cache_value[ tempIndex ];
// #endif
// 
//         fixed tempAngleSq = Multiply( fix16_t( tempAngle ), fix16_t( tempAngle ) );
// 
// #ifndef EE_FIXMATH_FAST_SIN // Most accurate version, accurate to ~2.1%
//         fixed tempAngle2 = (fix16_t)tempAngle;
//         fixed tempOut = tempAngle2;
// 
//         tempAngle2 *= tempAngleSq;
//         tempOut.value -= (tempAngle2.value / 6);
//         tempAngle2 *= tempAngleSq;
//         tempOut.value += (tempAngle2.value / 120);
//         tempAngle2 *= tempAngleSq;
//         tempOut.value -= (tempAngle2.value / 5040);
//         tempAngle2 *= tempAngleSq;
//         tempOut.value += (tempAngle2.value / 362880);
//         tempAngle2 *= tempAngleSq;
//         tempOut.value -= (tempAngle2.value / 39916800);
// #else // Fast implementation, runs at 159% the speed of above 'accurate' version with an slightly lower accuracy of ~2.3%
//         fixed tempOut;
//         fixed tempAngle2 = (fix16_t)-13;
//         tempAngle2 *= tempAngleSq;
//         tempOut.value = tempAngle2.value += 546;
//         tempOut *= tempAngleSq;
//         tempOut.value -= 10923;
//         tempOut *= tempAngleSq;
//         tempOut.value -= 65536;
//         tempOut *= (fix16_t)tempAngle;
// #endif
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         _fix16_sin_cache_index[ tempIndex ] = inAngle.value;
//         _fix16_sin_cache_value[ tempIndex ] = tempOut.value;
// #endif
// #endif
// 
//         return tempOut;
//     }
// 
//     fixed fixed::Cos( const fixed& inAngle ) noexcept
//     {
//         return Sin( (fix16_t)(inAngle.value + (FIX16_PI >> 1)) );
//     }
// 
//     fixed fixed::Tan( const fixed& inAngle ) noexcept
//     {
// #ifndef EE_FIXMATH_NO_OVERFLOW
//         return SaturateDivide( Sin( inAngle ), Cos( inAngle ) );
// #else
//         return Divide( Sin( inAngle ), Cos( inAngle ) );
// #endif
//     }
// 
//     fixed fixed::ASin( const fixed& inValue ) noexcept
//     {
//         if ( (inValue.value > FIX16_ONE) || (inValue.value < -FIX16_ONE) )
//             return (fix16_t)0;
//         fixed inValueSq = Multiply( inValue, inValue );
//         fixed tempOut;
//         tempOut = (fix16_t)(FIX16_ONE - inValueSq.value);
//         tempOut = inValue;
//         tempOut.Sqrt( tempOut );
//         tempOut = ATan( tempOut );
//         return tempOut;
//     }
// 
//     fixed fixed::ACos( const fixed& inValue ) noexcept
//     {
//         return (fix16_t)((FIX16_PI >> 1) - ASin( inValue ).value);
//     }
// 
//     fixed fixed::ATan2( const fixed& inY, const fixed& inX ) noexcept
//     {
//         int32 abs_inY, mask, angle;
//         fixed r, r_3;
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         uintptr_t hash = (inX.value ^ inY.value);
//         hash ^= hash >> 20;
//         hash &= 0x0FFF;
//         if ( (_fix16_atan_cache_index[ 0 ][ hash ] == inX.value) && (_fix16_atan_cache_index[ 1 ][ hash ] == inY.value) )
//             return (fix16_t)_fix16_atan_cache_value[ hash ];
// #endif
// 
//         /* Absolute inY */
//         mask = (inY.value >> (sizeof( int32 ) * CHAR_BIT - 1));
//         abs_inY = ((inY.value + mask) ^ mask);
// 
//         if ( inX.value >= 0 )
//         {
//             r = Divide( (fix16_t)(inX.value - abs_inY), (fix16_t)(inX.value + abs_inY) );
//             r_3 = ( ( r * r ) * r );
//             angle = ( (fixed)(fix16_t)0x00003240 * r_3 ).value - ( (fixed)(fix16_t)0x0000FB50 * r ).value + FIX16_QUARTER_PI;
//         }
//         else
//         {
//             r = Divide( (fix16_t)(inX.value + abs_inY), (fix16_t)(abs_inY - inX.value) );
//             r_3 = ( ( r * r ) * r );
//             angle = ( (fixed)(fix16_t)0x00003240 * r_3 ).value - ( (fixed)(fix16_t)0x0000FB50 * r ).value + FIX16_THREE_PI_DIV4;
//         }
//         if ( inY.value < 0 )
//         {
//             angle = -angle;
//         }
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         _fix16_atan_cache_index[ 0 ][ hash ] = inX.value;
//         _fix16_atan_cache_index[ 1 ][ hash ] = inY.value;
//         _fix16_atan_cache_value[ hash ] = angle;
// #endif
// 
//         return (fix16_t)angle;
//     }
// 
//     fixed fixed::ATan( const fixed& inValue ) noexcept
//     {
//         return ATan2( inValue, (fix16_t)FIX16_ONE );
//     }
// 
//     /* The square root algorithm is quite directly from
//     * http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
//     * An important difference is that it is split to two parts
//     * in order to use only 32-bit operations.
//     *
//     * Note that for negative numbers we return -sqrt(-inValue).
//     * Not sure if someone relies on this behaviour, but not going
//     * to break it for now. It doesn't slow the code much overall.
//     */
//     fixed fixed::Sqrt( const fixed& inValue ) noexcept
//     {
//         uint8 neg = (inValue.value < 0);
//         uint32 num = (neg ? -inValue.value : inValue.value);
//         uint32 result = 0;
//         uint32 bit;
//         uint8 n;
// 
//         // Many numbers will be less than 15, so
//         // this gives a good balance between time spent
//         // in if vs. time spent in the while loop
//         // when searching for the starting value.
//         if ( num & 0xFFF00000 )
//             bit = (uint32)1 << 30;
//         else
//             bit = (uint32)1 << 18;
// 
//         while ( bit > num ) bit >>= 2;
// 
//         // The main part is executed twice, in order to avoid
//         // using 64 bit values in computations.
//         for ( n = 0; n < 2; n++ )
//         {
//             // First we get the top 24 bits of the answer.
//             while ( bit )
//             {
//                 if ( num >= result + bit )
//                 {
//                     num -= result + bit;
//                     result = (result >> 1) + bit;
//                 }
//                 else
//                 {
//                     result = (result >> 1);
//                 }
//                 bit >>= 2;
//             }
// 
//             if ( n == 0 )
//             {
//                 // Then process it again to get the lowest 8 bits.
//                 if ( num > 65535 )
//                 {
//                     // The remainder 'num' is too large to be shifted left
//                     // by 16, so we have to add 1 to result manually and
//                     // adjust 'num' accordingly.
//                     // num = a - (result + 0.5)^2
//                     //   = num + result^2 - (result + 0.5)^2
//                     //   = num - result - 0.5
//                     num -= result;
//                     num = (num << 16) - 0x8000;
//                     result = (result << 16) + 0x8000;
//                 }
//                 else
//                 {
//                     num <<= 16;
//                     result <<= 16;
//                 }
// 
//                 bit = 1 << 14;
//             }
//         }
// 
// #ifndef EE_FIXMATH_NO_ROUNDING
//         // Finally, if next bit would have been 1, round the result upwards.
//         if ( num > result )
//         {
//             result++;
//         }
// #endif
// 
// #pragma warning( suppress : 4146 )
//         return (fix16_t)(neg ? -result : result);
//     }
// 
// #ifndef EE_FIXMATH_NO_CACHE
//     static int32 _fix16_exp_cache_index[ 4096 ] = { 0 };
//     static int32 _fix16_exp_cache_value[ 4096 ] = { 0 };
// #endif
// 
//     fixed fixed::Exp( const fixed& inValue ) noexcept
//     {
//         if ( inValue.value == 0 )
//             return (fix16_t)FIX16_ONE;
//         if ( inValue.value == FIX16_ONE )
//             return (fix16_t)FIX16_EPSILON;
//         if ( inValue.value > 681391 )
//             return (fix16_t)FIXED_MAX;
//         if ( inValue.value < -726817 )
//             return (fix16_t)0;
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         int32 tempIndex = (inValue.value ^ (inValue.value >> 16));
//         tempIndex = (inValue.value ^ (inValue.value >> 4)) & 0x0FFF;
//         if ( _fix16_exp_cache_index[ tempIndex ] == inValue.value )
//             return (fix16_t)_fix16_exp_cache_value[ tempIndex ];
// #endif
// 
//         int64 tempOut = ( (int64)( FIX16_ONE ) + (int64)( inValue.value ) );
//         int64 tempValue = inValue.value;
//         uint32 i, n;
//         for ( i = 3, n = 2; i < 13; n *= i, i++ )
//         {
//             tempValue = ( tempValue * inValue.value );
// #ifndef EE_FIXMATH_NO_ROUNDING
//             tempValue = ( tempValue + int64( FIX16_ONE >> 1 ) );
// #endif
// #pragma warning( suppress : 4293 )
//             tempValue = (-16 < 0 ? (tempValue >> 16) : (tempValue << -16));
//             tempOut = ( tempOut + ( tempValue / n ) );
//         }
// 
// #ifndef EE_FIXMATH_NO_CACHE
//         _fix16_exp_cache_index[ tempIndex ] = inValue.value;
//         _fix16_exp_cache_value[ tempIndex ] = (tempOut & ((1ULL << 32) - 1));
// #endif
// 
//         return (fix16_t)(tempOut & ((1ULL << 32) - 1));
//     }
}