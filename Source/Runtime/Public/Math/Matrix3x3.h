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
                struct { T c0r0, c0r1, c0r2, c1r0, c1r1, c1r2, c2r0, c2r1, c2r2; };
                TVector3<T> c0, c1, c2;
            };

            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3();
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TMatrix3x3<T>& matrix );
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TMatrix4x4<T>& matrix );
            HOST_DEVICE FORCEINLINE constexpr TMatrix3x3( const TVector3<T>& col0, const TVector3<T>& col1, const TVector3<T>& col2 );

            HOST_DEVICE constexpr static TMatrix3x3 Identity();

            HOST_DEVICE inline void Transpose();
            HOST_DEVICE inline TMatrix3x3 Transposed() const;

            HOST_DEVICE inline TVector3<T> GetRow( const unsigned char& i ) const;
            HOST_DEVICE inline TVector3<T> GetColumn( const unsigned char& i ) const;

            HOST_DEVICE inline TVector3<T>& operator[]( unsigned char i );
            HOST_DEVICE inline TVector3<T> const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE TMatrix3x3 operator*( const TMatrix3x3& other ) const;
            HOST_DEVICE FORCEINLINE TVector3<T> operator*( const TVector3<T>& vector ) const;
        };
    }
}

#include "Math/Matrix3x3.inl"
