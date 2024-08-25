#pragma once

#include "CoreTypes.h"

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TVector3
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T x, y, z; };
                struct { T r, g, b; };
            };

            HOST_DEVICE FORCEINLINE constexpr TVector3();
            HOST_DEVICE FORCEINLINE constexpr TVector3( const TVector2<T>& vector );
            template <typename I>
            HOST_DEVICE FORCEINLINE constexpr TVector3( const TIntVector3<I>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector3( const TVector3<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector3( const TVector4<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector3( const T& value );
            HOST_DEVICE FORCEINLINE constexpr TVector3( const T& x, const T& y, const T& z );
            HOST_DEVICE FORCEINLINE constexpr TVector3( const T& x, const T& y );

            HOST_DEVICE static constexpr TVector3<T> Right() { return TVector3<T>( T( 1 ), T( 0 ), T( 0 ) ); };
            HOST_DEVICE static constexpr TVector3<T> Left() { return TVector3<T>( T( -1 ), T( 0 ), T( 0 ) ); };
            HOST_DEVICE static constexpr TVector3<T> Up() { return TVector3<T>( T( 0 ), T( 1 ), T( 0 ) ); };
            HOST_DEVICE static constexpr TVector3<T> Down() { return TVector3<T>( T( 0 ), T( -1 ), T( 0 ) ); };
            HOST_DEVICE static constexpr TVector3<T> Forward() { return TVector3<T>( T( 0 ), T( 0 ), T( -1 ) ); };
            HOST_DEVICE static constexpr TVector3<T> Back() { return TVector3<T>( T( 0 ), T( 0 ), T( 1 ) ); };

            HOST_DEVICE inline T Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;
            HOST_DEVICE inline void Normalize();
            HOST_DEVICE inline TVector3<T> Normalized() const;

            HOST_DEVICE FORCEINLINE TVector3 Cross( const TVector3<T>& other ) const;
            HOST_DEVICE FORCEINLINE static TVector3 Cross( const TVector3<T>& a, const TVector3<T>& b );
            HOST_DEVICE FORCEINLINE T Dot( const TVector3<T>& other ) const;
            HOST_DEVICE FORCEINLINE static T Dot( const TVector3<T>& a, const TVector3<T>& b );
            HOST_DEVICE FORCEINLINE static TVector3<T> Lerp( const TVector3<T>& start, const TVector3<T>& end, T t );
            HOST_DEVICE FORCEINLINE static TVector3<T> Reflect( const TVector3<T>& incident, const TVector3<T>& normal );

            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TVector3& other ) const;
            HOST_DEVICE FORCEINLINE bool operator!=( const TVector3& other ) const;

            HOST_DEVICE FORCEINLINE TVector3 operator+( const TVector3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator-( const TVector3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator*( const TVector3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator/( const TVector3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TVector3 operator/( const T& value ) const;

            HOST_DEVICE FORCEINLINE TVector3& operator+=( const TVector3& other );
            HOST_DEVICE FORCEINLINE TVector3& operator-=( const TVector3& other );
            HOST_DEVICE FORCEINLINE TVector3& operator*=( const TVector3& other );
            HOST_DEVICE FORCEINLINE TVector3& operator/=( const TVector3& other );
            HOST_DEVICE FORCEINLINE TVector3& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TVector3& operator/=( const T& value );

            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector3 operator*( R value, const TVector3& vector ) 
            { return vector.operator*( value ); }
            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector3 operator/( R value, const TVector3& vector )
            { return TVector2( value / vector.x, value / vector.y, value / vector.z ); }

            template<typename R>
            explicit TVector3<T>( const TVector3<R>& other ) : TVector3<T>( (T)other.x, (T)other.y, (T)other.z ) {}
        };

        //* Get the angles in degrees in the range of (-180, 180) 
        template <typename T>
        inline TVector3<T> NormalizeAngleComponents( TVector3<T> degrees );

        //* Get the angles in degrees in the range of [0, 360)
        template <typename T>
        inline TVector3<T> ClampAngleComponents( TVector3<T> degrees );

    }
}

#include "Math/Vector3.inl"
