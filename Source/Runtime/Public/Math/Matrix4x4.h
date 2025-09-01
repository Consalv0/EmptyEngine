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
                struct { T c0r0, c0r1, c0r2, c0r3, c1r0, c1r1, c1r2, c1r3, c2r0, c2r1, c2r2, c2r3, c3r0, c3r1, c3r2, c3r3; };
                struct { TVector4<T> c0, c1, c2, c3; };
            };

            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4();
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4( const TMatrix4x4& other );
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4( const TVector4<T>& column0, const TVector4<T>& column1, const TVector4<T>& column2, const TVector4<T> column3 );
            HOST_DEVICE FORCEINLINE constexpr TMatrix4x4(
                T c0r0, T c0r1, T c0r2, T c0r3,
                T c1r0, T c1r1, T c1r2, T c1r3,
                T c2r0, T c2r1, T c2r2, T c2r3,
                T c3r0, T c3r1, T c3r2, T c3r3
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
            HOST_DEVICE inline TVector4<T>& GetColumn( const unsigned char& i ) const;
            HOST_DEVICE inline TVector3<T> ExtractScale() const;
            HOST_DEVICE inline TVector3<T> ExtractTranslation() const;
            HOST_DEVICE inline TQuaternion<T> ExtractRotation() const;

            HOST_DEVICE inline TVector4<T>& operator[]( unsigned char i );
            HOST_DEVICE inline TVector4<T> const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE inline TVector4<T> MultiplyPoint( const TVector3<T>& vector ) const;
            HOST_DEVICE inline TVector4<T> MultiplyPoint( const TVector4<T>& vector ) const;
            HOST_DEVICE inline TVector4<T> MultiplyPointTransposed( const TVector4<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4<T> Multiply( const TMatrix4x4<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4<T> MultiplyTransposed( const TMatrix4x4<T>& vector ) const;
            HOST_DEVICE inline TVector4<T> Multiply( const TVector4<T>& vector ) const;
            HOST_DEVICE inline TVector4<T> MultiplyTransposed( const TVector4<T>& vector ) const;
            HOST_DEVICE inline TVector3<T> MultiplyVector( const TVector3<T>& vector ) const;

            HOST_DEVICE FORCEINLINE TMatrix4x4 operator*( const TMatrix4x4& other ) const;
            HOST_DEVICE FORCEINLINE TVector4<T> operator*( const TVector4<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TVector3<T> operator*( const TVector3<T>& vector ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TMatrix4x4 operator/( const T& value ) const;

            template<typename R>
            explicit TMatrix4x4<T>( const TMatrix4x4<R>& other ) : TMatrix4x4<T>( 
                T( other.c0r0 ), T( other.c0r1 ), T( other.c0r2 ), T( other.c0r3 ),
                T( other.c1r0 ), T( other.c1r1 ), T( other.c1r2 ), T( other.c1r3 ),
                T( other.c2r0 ), T( other.c2r1 ), T( other.c2r2 ), T( other.c2r3 ),
                T( other.c3r0 ), T( other.c3r1 ), T( other.c3r2 ), T( other.c3r3 ) ) {}
        };
    }
}

#include "Math/Matrix4x4.inl"
