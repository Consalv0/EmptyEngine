#pragma once

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TMatrix4x4
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33; };
                struct { TVector4<T> m0, m1, m2, m3; };
                struct { TVector4<T> x, y, z, w; };
            };

            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4();
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4( const TMatrix4x4& other );
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4( const TVector4<T>& row0, const TVector4<T>& row1, const TVector4<T>& row2, const TVector4<T> Row3 );
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4(
                T m00, T m01, T m02, T m03,
                T m10, T m11, T m12, T m13,
                T m20, T m21, T m22, T m23,
                T m30, T m31, T m32, T m33
            );

            HOST_DEVICE inline static TMatrix4x4 Identity();

            //* Creates a perspective matrix, FOV is the aperture angle in radians
            HOST_DEVICE inline static TMatrix4x4 Perspective( const T& aperture, const T& aspect, const T& near, const T& far );
            //* Creates a perspective matrix, FOV is the aperture angle in radians with z-value reversed
            HOST_DEVICE inline static TMatrix4x4 PerspectiveReversed( const T& aperture, const T& aspect, const T& near, const T& far );
            //* Creates a orthographic matrix
            HOST_DEVICE inline static TMatrix4x4 Orthographic( const T& left, const T& right, const T& bottom, const T& top );
            //* Creates a orthographic matrix with zClamping
            HOST_DEVICE inline static TMatrix4x4 Orthographic( const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far );
            HOST_DEVICE inline static TMatrix4x4 LookAt( const TVector3<T>& position, const TVector3<T>& direction, const TVector3<T>& up );
            HOST_DEVICE inline static TMatrix4x4 Translation( const TVector3<T>& vector );
            HOST_DEVICE inline static TMatrix4x4 Scaling( const TVector3<T>& vector );
            HOST_DEVICE inline static TMatrix4x4 Rotation( const TVector3<T>& axis, const T& angle );
            HOST_DEVICE inline static TMatrix4x4 Rotation( const TVector3<T>& eulerAngles );
            HOST_DEVICE inline static TMatrix4x4 Rotation( const TQuaternion<T>& quat );

            HOST_DEVICE inline void Transpose();
            HOST_DEVICE inline TMatrix4x4 Transposed() const;
            HOST_DEVICE inline TMatrix4x4 Inversed() const;

            HOST_DEVICE inline TVector4<T> GetRow( const unsigned char& i ) const;
            HOST_DEVICE inline TVector4<T> GetColumn( const unsigned char& i ) const;
            HOST_DEVICE inline TVector3<T> ExtractScale() const;
            HOST_DEVICE inline TVector3<T> ExtractTranslation() const;
            HOST_DEVICE inline TQuaternion<T> ExtractRotation() const;

            HOST_DEVICE inline TVector4<T>& operator[]( unsigned char i );
            HOST_DEVICE inline TVector4<T> const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue( void ) const;

            HOST_DEVICE inline TVector4<T> MultiplyPoint( const TVector3<T>& vector ) const;
            HOST_DEVICE inline TVector3<T> MultiplyVector( const TVector3<T>& vector ) const;

            HOST_DEVICE FORCEINLINE TMatrix4x4 operator*( const TMatrix4x4& other ) const;
            HOST_DEVICE FORCEINLINE TVector4<T> operator*( const TVector4<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TVector3<T> operator*( const TVector3<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4 operator/( const T& value ) const;

            template<typename R>
            explicit TMatrix4x4<T>( const TMatrix4x4<R>& other ) : TMatrix4x4<T>( 
                T( other.m00 ), T( other.m01 ), T( other.m02 ), T( other.m03 ),
                T( other.m10 ), T( other.m11 ), T( other.m12 ), T( other.m13 ),
                T( other.m20 ), T( other.m21 ), T( other.m22 ), T( other.m23 ),
                T( other.m30 ), T( other.m31 ), T( other.m32 ), T( other.m33 ) ) {}
        };
    }
}

#include "Math/Matrix4x4.inl"
