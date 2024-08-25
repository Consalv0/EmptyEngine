#pragma once

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TMatrix3x3
        {
            static_assert(std::is_floating_point_v<T>, "T must be floating point.");

        public:
            struct
            {
                struct { T m00, m01, m02, m10, m11, m12, m20, m21, m22; };
                TVector3<T> m0, m1, m2;
            };

            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3();
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TMatrix3x3<T>& matrix );
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TMatrix4x4<T>& matrix );
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TVector3<T>&row0, const TVector3<T>& row1, const TVector3<T>& row2 );

            HOST_DEVICE constexpr static TMatrix3x3 Identity();

            HOST_DEVICE inline void Transpose();
            HOST_DEVICE inline TMatrix3x3 Transposed() const;

            HOST_DEVICE inline TVector3<T> GetRow( const unsigned char& i ) const;
            HOST_DEVICE inline TVector3<T> GetColumn( const unsigned char& i ) const;

            HOST_DEVICE inline TVector3<T>& operator[]( unsigned char i );
            HOST_DEVICE inline TVector3<T> const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue( void ) const;

            HOST_DEVICE FORCEINLINE TMatrix3x3 operator*( const TMatrix3x3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3<T> operator*( const TVector3<T>& vector ) const;
        };
    }
}

#include "Math/Matrix3x3.inl"
