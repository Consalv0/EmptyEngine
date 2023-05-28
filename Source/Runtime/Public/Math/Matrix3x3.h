#pragma once

namespace EE
{
	struct Vector3;
	struct Matrix4x4;

	struct Matrix3x3
	{
	public:
		struct
		{
			Vector3 m0, m1, m2;
		};

		HOST_DEVICE FORCEINLINE Matrix3x3();
		HOST_DEVICE FORCEINLINE Matrix3x3( const Matrix3x3& matrix );
		HOST_DEVICE FORCEINLINE Matrix3x3( const Matrix4x4& matrix );
		HOST_DEVICE FORCEINLINE Matrix3x3( const Vector3& row0, const Vector3& row1, const Vector3& row2 );

		HOST_DEVICE inline static Matrix3x3 Identity();

		HOST_DEVICE inline void Transpose();
		HOST_DEVICE inline Matrix3x3 Transposed() const;

		HOST_DEVICE inline Vector3 GetRow( const unsigned char& i ) const;
		HOST_DEVICE inline Vector3 GetColumn( const unsigned char& i ) const;

		HOST_DEVICE inline Vector3& operator[]( unsigned char i );
		HOST_DEVICE inline Vector3 const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const float* PointerToValue( void ) const;

		HOST_DEVICE FORCEINLINE Matrix3x3 operator*( const Matrix3x3& other ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator*( const Vector3& vector ) const;
	};
}

#include "Math/Matrix3x3.inl"
