#pragma once

namespace EEngine
{
	struct Vector4;
	struct Vector3;
	struct Quaternion;

	struct Matrix4x4
	{

	public:
		union
		{
			struct { Vector4 m0, m1, m2, m3; };
			struct { Vector4 x, y, z, w; };
		};

		HOST_DEVICE FORCEINLINE Matrix4x4();
		HOST_DEVICE FORCEINLINE Matrix4x4( const Matrix4x4& other );
		HOST_DEVICE FORCEINLINE Matrix4x4( const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4 Row3 );
		HOST_DEVICE FORCEINLINE Matrix4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		);

		HOST_DEVICE inline static Matrix4x4 Identity();

		//* Creates a perspective matrix, FOV is the aperture angle in radians
		HOST_DEVICE inline static Matrix4x4 Perspective( const float& aperture, const float& aspect, const float& near, const float& far );
		//* Creates a orthographic matrix
		HOST_DEVICE inline static Matrix4x4 Orthographic( const float& left, const float& right, const float& bottom, const float& top );
		//* Creates a orthographic matrix with zClamping
		HOST_DEVICE inline static Matrix4x4 Orthographic( const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far );
		HOST_DEVICE inline static Matrix4x4 LookAt( const Vector3& position, const Vector3& direction, const Vector3& up );
		HOST_DEVICE inline static Matrix4x4 Translation( const Vector3& vector );
		HOST_DEVICE inline static Matrix4x4 Scaling( const Vector3& vector );
		HOST_DEVICE inline static Matrix4x4 Rotation( const Vector3& axis, const float& angle );
		HOST_DEVICE inline static Matrix4x4 Rotation( const Vector3& eulerAngles );
		HOST_DEVICE inline static Matrix4x4 Rotation( const Quaternion& quat );

		HOST_DEVICE inline void Transpose();
		HOST_DEVICE inline Matrix4x4 Transposed() const;
		HOST_DEVICE inline Matrix4x4 Inversed() const;

		HOST_DEVICE inline Vector4 GetRow( const unsigned char& i ) const;
		HOST_DEVICE inline Vector4 GetColumn( const unsigned char& i ) const;
		HOST_DEVICE inline Vector3 ExtractScale() const;
		HOST_DEVICE inline Vector3 ExtractTranslation() const;
		HOST_DEVICE inline Quaternion ExtractRotation() const;

		HOST_DEVICE inline Vector4& operator[]( unsigned char i );
		HOST_DEVICE inline Vector4 const& operator[]( unsigned char i ) const;
		HOST_DEVICE inline const float* PointerToValue( void ) const;

		HOST_DEVICE inline Vector3 MultiplyPoint( const Vector3& vector ) const;
		HOST_DEVICE inline Vector3 MultiplyVector( const Vector3& vector ) const;

		HOST_DEVICE FORCEINLINE Matrix4x4 operator*( const Matrix4x4& other ) const;
		HOST_DEVICE FORCEINLINE Vector4 operator*( const Vector4& vector ) const;
		HOST_DEVICE FORCEINLINE Vector3 operator*( const Vector3& vector ) const;
		HOST_DEVICE FORCEINLINE Matrix4x4 operator*( const float& value ) const;
		HOST_DEVICE FORCEINLINE Matrix4x4 operator/( const float& value ) const;
	};
}

#include "Math/Matrix4x4.inl"
