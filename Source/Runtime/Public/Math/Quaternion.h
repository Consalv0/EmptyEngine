#pragma once

namespace EE
{
    namespace Math
    {
        enum AngleAxes
        {
            Pitch = 0, // x forward to up
            Yaw = 1,   // y right to forward
            Roll = 2   // z up to right
        };

        template <typename T>
        struct TQuaternion
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T w, x, y, z; };
                struct { T scalar; TVector3<T> vector; };
            };

            HOST_DEVICE FORCEINLINE constexpr TQuaternion();
            HOST_DEVICE FORCEINLINE constexpr TQuaternion( TQuaternion const& other );
            HOST_DEVICE FORCEINLINE constexpr TQuaternion( T const& scalar, TVector3<T> const& vector );
            HOST_DEVICE FORCEINLINE constexpr TQuaternion( T const& w, T const& x, T const& y, T const& z );

            //* Create a quaternion from euler angles (pitch, yaw, roll).
            HOST_DEVICE static FORCEINLINE TQuaternion FromEulerAngles( TVector3<T> const& degrees );
            //* Create a quaternion from two normalized axis
            HOST_DEVICE static FORCEINLINE TQuaternion FromToRotation( TVector3<T> const& from, TVector3<T> const& To );
            HOST_DEVICE static FORCEINLINE TQuaternion FromAxisAngle( TVector3<T> const& axis, T const& radians );
            //* Ctreate quaternion from two basis vectors
            HOST_DEVICE static FORCEINLINE TQuaternion FromLookRotation( TVector3<T> const& forward, TVector3<T> const& up );
            HOST_DEVICE static FORCEINLINE TQuaternion FromMatrix( TMatrix3x3<T> const& matrix );

            HOST_DEVICE static void Interpolate( const TQuaternion& start, const TQuaternion& end, T factor, TQuaternion& out );

            HOST_DEVICE inline T Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;
            HOST_DEVICE inline void Normalize();
            HOST_DEVICE inline TQuaternion Normalized() const;
            HOST_DEVICE inline TQuaternion Conjugated() const;
            HOST_DEVICE inline TQuaternion Inversed() const;

            HOST_DEVICE inline TMatrix4x4<T> ToMatrix4x4() const;
            
            //* Deconstruct quaternion to euler angles pitch (degrees)
            HOST_DEVICE inline T GetPitch() const;
            //* Deconstruct quaternion to euler angles yaw (degrees)
            HOST_DEVICE inline T GetYaw() const;
            //* Deconstruct quaternion to euler angles roll (degrees)
            HOST_DEVICE inline T GetRoll() const;
            //* Same as TQuaternion * TVector3::Right()
            HOST_DEVICE inline TVector3<T> Right() const;
            //* Same as TQuaternion * TVector3::Up()
            HOST_DEVICE inline TVector3<T> Up() const;
            //* Same as TQuaternion * TVector3::Forward()
            HOST_DEVICE inline TVector3<T> Forward() const;

            HOST_DEVICE inline T GetScalar() const;
            HOST_DEVICE inline TVector3<T> GetVector() const;
            //* Deconstruct quaternion to euler angles (degrees)
            HOST_DEVICE inline TVector3<T> ToEulerAngles() const;

            HOST_DEVICE FORCEINLINE T Dot( const TQuaternion& other ) const;
            HOST_DEVICE FORCEINLINE TQuaternion Cross( const TQuaternion& other ) const;

            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TQuaternion& other ) const;
            HOST_DEVICE FORCEINLINE bool operator!=( const TQuaternion& other ) const;

            HOST_DEVICE FORCEINLINE TQuaternion operator-( void ) const;
            HOST_DEVICE FORCEINLINE TQuaternion operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TQuaternion operator/( const T& value ) const;
            HOST_DEVICE FORCEINLINE TQuaternion operator*( const TQuaternion& other ) const;
            HOST_DEVICE FORCEINLINE TVector3<T> operator*( const TVector3<T>& vector ) const;

            HOST_DEVICE FORCEINLINE TQuaternion& operator*=( const TQuaternion& other );
            HOST_DEVICE FORCEINLINE TQuaternion& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TQuaternion& operator/=( const T& value );

            template<typename R>
            explicit TQuaternion<T>( const TQuaternion<R>& other ) : TQuaternion<T>( (T)other.w, (T)other.x, (T)other.y, (T)other.z ) {}
        };
    }
}

#include "Math/Quaternion.inl"
