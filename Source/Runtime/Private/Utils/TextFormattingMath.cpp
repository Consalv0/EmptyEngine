
#include "CoreMinimal.h"

#include "Utils/TextFormattingMath.h"

namespace EE
{
    // Original crack_atof version is at http://crackprogramming.blogspot.sg/2012/10/implement-atof.html
    // But it cannot convert floating point with high +/- exponent.
    // The version below by Tian Bo fixes that problem and improves performance by 10%
    // http://coliru.stacked-crooked.com/a/2e28f0d71f47ca5e
    float Text::ParseFloat( std::string_view* text )
    {
        if ( text->empty() )
            return 0;

        int sign = 1;
        float intPart = 0.0;
        float fractionPart = 0.0;
        bool hasFraction = false;
        bool hasExpo = false;

        // Take care of +/- sign
        if ( (*text)[ 0 ] == '-' ) { text->remove_prefix( 1 ); sign = -1; }
        else if ( (*text)[ 0 ] == '+' ) text->remove_prefix( 1 );

        while ( text->empty() != true && (*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ' )
        {
            if ( (*text)[ 0 ] >= '0' && (*text)[ 0 ] <= '9' )
                intPart = intPart * 10 + ((*text)[ 0 ] - '0');
            else if ( (*text)[ 0 ] == '.' )
            {
                hasFraction = true;
                text->remove_prefix( 1 ); break;
            }
            else if ( (*text)[ 0 ] == 'e' )
            {
                hasExpo = true;
                text->remove_prefix( 1 ); break;
            }
            else
            {
                break;
            }
            text->remove_prefix( 1 );
        }

        if ( hasFraction )
        {
            float fractionExponential = 0.1F;

            while ( text->empty() != true && (*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ' )
            {
                if ( (*text)[ 0 ] >= '0' && (*text)[ 0 ] <= '9' )
                {
                    fractionPart += fractionExponential * ((*text)[ 0 ] - '0');
                    fractionExponential *= 0.1F;
                }
                else if ( (*text)[ 0 ] == 'e' )
                {
                    hasExpo = true;
                    text->remove_prefix( 1 );
                    break;
                }
                else
                {
                    return sign * (intPart + fractionPart);
                }
                text->remove_prefix( 1 );
            }
        }

        float exponentPart = 1.0;
        if ( hasExpo && text->empty() != true && ((*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ') )
        {
            int32 exponentSign = 1;
            if ( (*text)[ 0 ] == '-' )
            {
                exponentSign = -1;
                text->remove_prefix( 1 );
            }
            else if ( (*text)[ 0 ] == '+' )
            {
                text->remove_prefix( 1 );
            }

            int32 e = 0;
            while ( text->empty() != true && ((*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ') && (*text)[ 0 ] >= '0' && (*text)[ 0 ] <= '9' )
            {
                e = e * 10 + (*text)[ 0 ] - '0';
                text->remove_prefix( 1 );
            }

            exponentPart = Math::Pow10<float, int32>( exponentSign * e );
        }

        if ( text->empty() == false )
            text->remove_prefix( 1 );

        return sign * (intPart + fractionPart) * exponentPart;
    }

    int32 Text::ParseInt( std::string_view* text )
    {
        if ( text->empty() )
            return 0;

        int32 sign = 1;
        int32 intPart = 0;

        // Take care of +/- sign
        if ( (*text)[ 0 ] == '-' ) { text->remove_prefix( 1 ); sign = -1; }
        else if ( (*text)[ 0 ] == '+' ) text->remove_prefix( 1 );

        while ( text->empty() != true && (*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ' )
        {
            if ( (*text)[ 0 ] >= '0' && (*text)[ 0 ] <= '9' )
                intPart = intPart * 10 + ((*text)[ 0 ] - '0');
            else
                break;
            text->remove_prefix( 1 );
        }

        if ( text->empty() == false && (*text)[ 0 ] != '\0' && (*text)[ 0 ] != ',' && (*text)[ 0 ] != ' ' )
            text->remove_prefix( 1 );

        return sign * intPart;
    }
}
