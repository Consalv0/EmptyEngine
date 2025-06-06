
namespace EE::Math
{
    template <typename T>
    FORCEINLINE constexpr TMatrix4x4<T>::TMatrix4x4() :
        m00(1), m01(0), m02(0), m03(0),
        m10(0), m11(1), m12(0), m13(0),
        m20(0), m21(0), m22(1), m23(0),
        m30(0), m31(0), m32(0), m33(1)
    {
    }

    template <typename T>
    FORCEINLINE constexpr TMatrix4x4<T>::TMatrix4x4( const TMatrix4x4& other )
        : c0( other.c0 ), c1( other.c1 ), c2( other.c2 ), c3( other.c3 )
    {
    }

    template <typename T>
    FORCEINLINE constexpr TMatrix4x4<T>::TMatrix4x4( const TVector4<T>& col0, const TVector4<T>& col1, const TVector4<T>& col2, const TVector4<T> col3 )
    {
        m00 = col0.x; m01 = col0.y; m02 = col0.z; m03 = col0.w;
        m10 = col1.x; m11 = col1.y; m12 = col1.z; m13 = col1.w;
        m20 = col2.x; m21 = col2.y; m22 = col2.z; m23 = col2.w;
        m30 = col3.x; m31 = col3.y; m32 = col3.z; m33 = col3.w;
    }

    template <typename T>
    inline constexpr TMatrix4x4<T>::TMatrix4x4(
        T m00, T m01, T m02, T m03,
        T m10, T m11, T m12, T m13,
        T m20, T m21, T m22, T m23,
        T m30, T m31, T m32, T m33 ) :
        m00(m00), m01(m01), m02(m02), m03(m03),
        m10(m10), m11(m11), m12(m12), m13(m13),
        m20(m20), m21(m21), m22(m22), m23(m23),
        m30(m30), m31(m31), m32(m32), m33(m33)
    {
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Identity()
    {
        return TMatrix4x4();
    }

    // inline Matrix4x4 Matrix4x4::Perspective( const T& fov, const T& aspect, const T& near, const T& far )
    // {
    //     T const tanHalfFOV = tan( fov / 2.F );
    //     Matrix4x4 result(
    //         1.F / (aspect * tanHalfFOV), 0.F, 0.F, 0.F,
    //         0.F, 1.F / (tanHalfFOV), 0.F, 0.F,
    //         0.F, 0.F, -(far + near) / (far - near), 1.F,
    //         0.F, 0.F, -(2.F * far * near) / (far - near), 0.F
    //     );
    //     return result;
    // }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::PerspectiveReversed( const T& fov, const T& aspect, const T& near, const T& far )
    {
        // float fov_rad = vertical_fov * 2.0f * M_PI / 360.0f;
        // float focal_length = 1.0f / std::tan( fov_rad / 2.0f );
        // 
        // float x = focal_length / aspect_ratio;
        // float y = -focal_length;
        // float A = n / (f - n);
        // float B = f * A;
        // 
        // float4x4 projection( {
        //     x,    0.0f,  0.0f, 0.0f,
        //     0.0f,    y,  0.0f, 0.0f,
        //     0.0f, 0.0f,     A,    B,
        //     0.0f, 0.0f, -1.0f, 0.0f,
        //     } );

        T const focalLength = Math::Tan( fov * T( 0.5 ) );
        T const width = T( 1 ) / (focalLength * aspect);
        T const height = T( 1 ) / (focalLength);
        T const farRange = ((near == far) ? T( 1 ) : far / (far - near));
        T const nearRange = ((near == far) ? -near : -near * far / (far - near));

        TMatrix4x4 result(
            width, T(0), T(0), T(0),
            T(0), height, T(0), T(0),
            T(0), T(0), farRange, T(1),
            T(0), T(0), nearRange, T(0)
        );
        return result;
    }

    // Depth 0 to 1
    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Perspective( const T& fov, const T& aspect, const T& near, const T& far )
    {
        T const focalLength = Math::Tan( fov * T( 0.5 ) );
        T const width = T( 1 ) / (focalLength * aspect);
        T const height = T( 1 ) / (focalLength);
        T const farRange = ((near == far) ? T( 0 ) : near / (near - far));
        T const nearRange = ((near == far) ? near : (-far * near) / (near - far));

        TMatrix4x4 result(
            width, T(0), T(0), T(0),
            T(0), height, T(0), T(0),
            T(0), T(0), farRange, T(1),
            T(0), T(0), nearRange, T(0)
        );
        return result;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Orthographic( const T& left, const T& right, const T& bottom, const T& top )
    {
        TMatrix4x4 result(
            T(2) / (right - left), T(0), T(0), T(0),
            T(0), T(2) / (top - bottom), T(0), T(0),
            T(0), T(0), T(1), T(0),
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), T(0), T(1)
        );
        return result;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Orthographic( const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far )
    {
        TMatrix4x4 result(
            T(2) / (right - left), T(0), T(0), T(0),
            T(0), T(2) / (top - bottom), T(0), T(0),
            T(0), T(0), T(1) / (far - near), T(0),
            -(right + left) / (right - left), -(top + bottom) / (top - bottom), near / (far - near), T(1)
        );
        return result;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::LookAt( const TVector3<T>& eye, const TVector3<T>& target, const TVector3<T>& up )
    {
        TVector3<T> const forward( (eye - target).Normalized() );
        TVector3<T> const side( forward.Cross( up ).Normalized() );
        TVector3<T> const upper( side.Cross( forward ) );

        return TMatrix4x4(
            side.x, side.y, side.z, T(0),
            upper.x, upper.y, upper.z, T(0),
            forward.x, forward.y, forward.z, T(0),
            -eye.x, -eye.y, -eye.z, T(1)
        );
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Translation( const TVector3<T>& vector )
    {
        return TMatrix4x4(
            T(1), T(0), T(0), T(0),
            T(0), T(1), T(0), T(0),
            T(0), T(0), T(1), T(0),
            vector.x, vector.y, vector.z, T(1)
        );
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Scaling( const TVector3<T>& vector )
    {
        return TMatrix4x4(
            vector.x, T(0), T(0), T(0),
            T(0), vector.y, T(0), T(0),
            T(0), T(0), vector.z, T(0),
            T(0), T(0), T(0), T(1)
        );
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Rotation( const TVector3<T>& axis, const T& radians )
    {
        T const cosA = cos( radians );
        T const sinA = sin( radians );

        TVector3<T> axisN( axis.Normalized() );
        TVector3<T> temp( axisN * (T( 1 ) - cosA) );

        TMatrix4x4 rotation;
        rotation.m00 = cosA + temp.x * axisN.x;
        rotation.m01 = temp.x * axisN.y + sinA * axisN.z;
        rotation.m02 = temp.x * axisN.z - sinA * axisN.y;

        rotation.m10 = temp.y * axisN.x - sinA * axisN.z;
        rotation.m11 = cosA + temp.y * axisN.y;
        rotation.m12 = temp.y * axisN.z + sinA * axisN.x;

        rotation.m20 = temp.z * axisN.x + sinA * axisN.y;
        rotation.m21 = temp.z * axisN.y - sinA * axisN.x;
        rotation.m22 = cosA + temp.z * axisN.z;

        return rotation;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Rotation( const TVector3<T>& euler )
    {
        TMatrix4x4 result;
        T sinR, sinP, sinY, cosR, cosP, cosY;

        sinY = Math::Sin( euler[ Yaw ] * MathConstants<T>::DegToRad );
        cosY = Math::Cos( euler[ Yaw ] * MathConstants<T>::DegToRad );
        sinP = Math::Sin( euler[ Pitch ] * MathConstants<T>::DegToRad );
        cosP = Math::Cos( euler[ Pitch ] * MathConstants<T>::DegToRad );
        sinR = Math::Sin( euler[ Roll ] * MathConstants<T>::DegToRad );
        cosR = Math::Cos( euler[ Roll ] * MathConstants<T>::DegToRad );

        result.m00 = cosP * cosY;
        result.m01 = cosP * sinY;
        result.m02 = -sinP;

        result.m10 = sinR * sinP * cosY + cosR * -sinY;
        result.m11 = sinR * sinP * sinY + cosR * cosY;
        result.m12 = sinR * cosP;

        result.m20 = (cosR * sinP * cosY + -sinR * -sinY);
        result.m21 = (cosR * sinP * sinY + -sinR * cosY);
        result.m22 = cosR * cosP;

        result.m30 = T(0);
        result.m31 = T(0);
        result.m32 = T(0);
        result.m33 = T(1);

        return result;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Rotation( const TQuaternion<T>& quat )
    {
        return quat.ToMatrix4x4();
    }

    template <typename T>
    inline void TMatrix4x4<T>::Transpose()
    {
        TMatrix4x4 result = TMatrix4x4( GetRow( 0 ), GetRow( 1 ), GetRow( 2 ), GetRow( 3 ) );
        *this = result;
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Transposed() const
    {
        return TMatrix4x4( GetRow( 0 ), GetRow( 1 ), GetRow( 2 ), GetRow( 3 ) );
    }

    template <typename T>
    inline TMatrix4x4<T> TMatrix4x4<T>::Inversed() const
    {
        T coef00 = m22 * m33 - m32 * m23;
        T coef02 = m12 * m33 - m32 * m13;
        T coef03 = m12 * m23 - m22 * m13;

        T coef04 = m21 * m33 - m31 * m23;
        T coef06 = m11 * m33 - m31 * m13;
        T coef07 = m11 * m23 - m21 * m13;

        T coef08 = m21 * m32 - m31 * m22;
        T coef10 = m11 * m32 - m31 * m12;
        T coef11 = m11 * m22 - m21 * m12;

        T coef12 = m20 * m33 - m30 * m23;
        T coef14 = m10 * m33 - m30 * m13;
        T coef15 = m10 * m23 - m20 * m13;

        T coef16 = m20 * m32 - m30 * m22;
        T coef18 = m10 * m32 - m30 * m12;
        T coef19 = m10 * m22 - m20 * m12;

        T coef20 = m20 * m31 - m30 * m21;
        T coef22 = m10 * m31 - m30 * m11;
        T coef23 = m10 * m21 - m20 * m11;

        TVector4<T> fac0( coef00, coef00, coef02, coef03 );
        TVector4<T> fac1( coef04, coef04, coef06, coef07 );
        TVector4<T> fac2( coef08, coef08, coef10, coef11 );
        TVector4<T> fac3( coef12, coef12, coef14, coef15 );
        TVector4<T> fac4( coef16, coef16, coef18, coef19 );
        TVector4<T> fac5( coef20, coef20, coef22, coef23 );

        TVector4<T> vec0( m10, m00, m00, m00 );
        TVector4<T> vec1( m11, m01, m01, m01 );
        TVector4<T> vec2( m12, m02, m02, m02 );
        TVector4<T> vec3( m13, m03, m03, m03 );

        TVector4<T> inv0( vec1 * fac0 - vec2 * fac1 + vec3 * fac2 );
        TVector4<T> inv1( vec0 * fac0 - vec2 * fac3 + vec3 * fac4 );
        TVector4<T> inv2( vec0 * fac1 - vec1 * fac3 + vec3 * fac5 );
        TVector4<T> inv3( vec0 * fac2 - vec1 * fac4 + vec2 * fac5 );

        TVector4<T> signA( +1, -1, +1, -1 );
        TVector4<T> signB( -1, +1, -1, +1 );

        TMatrix4x4 result( inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB );

        // TVector4<T> row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

        TVector4<T> dot0( GetColumn( 0 ) * result.GetRow( 0 ) );
        T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        T oneOverDeterminant = T(1) / dot1;

        return result * oneOverDeterminant;
    }

    template <typename T>
    inline TVector4<T> TMatrix4x4<T>::GetRow( const unsigned char& i ) const
    {
        switch ( i )
        {
            case 0: return { m00, m10, m20, m30 };
            case 1: return { m01, m11, m21, m31 };
            case 2: return { m02, m12, m22, m32 };
            case 3: return { m03, m13, m23, m33 };
        }

        return TVector4<T>();
    }

    template <typename T>
    inline TVector4<T> TMatrix4x4<T>::GetColumn( const unsigned char& i ) const
    {
        if ( i > 3 ) return TVector4<T>();
        return ((TVector4<T>*)this)[ i ];
    }

    template <typename T>
    inline HOST_DEVICE TVector3<T> TMatrix4x4<T>::ExtractTranslation() const
    {
        return GetColumn( 3 );
    }

    template <typename T>
    inline HOST_DEVICE TQuaternion<T> TMatrix4x4<T>::ExtractRotation() const
    {
        return TQuaternion<T>::FromLookRotation( GetColumn( 2 ), GetColumn( 1 ) );
    }

    template <typename T>
    inline HOST_DEVICE TVector3<T> TMatrix4x4<T>::ExtractScale() const
    {
        TVector3<T> scale(
            GetColumn( 0 ).Magnitude(),
            GetColumn( 1 ).Magnitude(),
            GetColumn( 2 ).Magnitude()
        );
        return scale;
    }

    template <typename T>
    inline TVector4<T>& TMatrix4x4<T>::operator[]( unsigned char i )
    {
        EE_ASSERT( i <= 3, "TMatrix4x4 index out of bounds" );
        return ((TVector4<T>*)this)[ i ];
    }

    template <typename T>
    inline TVector4<T> const& TMatrix4x4<T>::operator[]( unsigned char i ) const
    {
        EE_ASSERT( i <= 3, "TMatrix4x4 index out of bounds" );
        return ((TVector4<T>*)this)[ i ];
    }

    template <typename T>
    inline HOST_DEVICE TVector4<T> TMatrix4x4<T>::MultiplyPoint( const TVector4<T>& vector ) const
    {
        TVector4<T> result(
            GetRow( 0 ).Dot( vector ),
            GetRow( 1 ).Dot( vector ),
            GetRow( 2 ).Dot( vector ),
            GetRow( 3 ).Dot( vector )
        );

        result.w = T(1) / result.w;
        result.x *= result.w;
        result.y *= result.w;
        result.z *= result.w;
        result.w = T(1);
        return result;
    }

    template <typename T>
    inline HOST_DEVICE TVector4<T> TMatrix4x4<T>::MultiplyPointTransposed( const TVector4<T>& vector ) const
    {
        TVector4<T> result(
            GetColumn( 0 ).Dot( vector ),
            GetColumn( 1 ).Dot( vector ),
            GetColumn( 2 ).Dot( vector ),
            GetColumn( 3 ).Dot( vector )
        );

        result.w = T( 1 ) / result.w;
        result.x *= result.w;
        result.y *= result.w;
        result.z *= result.w;
        result.w = T( 1 );
        return result;
    }

    template <typename T>
    FORCEINLINE TMatrix4x4<T> TMatrix4x4<T>::Multiply( const TMatrix4x4& other ) const
    {
        const TVector4<T> &row0 = GetRow( 0 ), &row1 = GetRow( 1 ), &row2 = GetRow( 2 ), &row3 = GetRow( 3 );
        const TVector4<T> &col0 = other.GetColumn( 0 ), &col1 = other.GetColumn( 1 ), &col2 = other.GetColumn( 2 ), &col3 = other.GetColumn( 3 );

        const T& m00 = TVector4<T>::Dot( col0, row0 );
        const T& m10 = TVector4<T>::Dot( col1, row0 );
        const T& m20 = TVector4<T>::Dot( col2, row0 );
        const T& m30 = TVector4<T>::Dot( col3, row0 );

        const T& m01 = TVector4<T>::Dot( col0, row1 );
        const T& m11 = TVector4<T>::Dot( col1, row1 );
        const T& m21 = TVector4<T>::Dot( col2, row1 );
        const T& m31 = TVector4<T>::Dot( col3, row1 );

        const T& m02 = TVector4<T>::Dot( col0, row2 );
        const T& m12 = TVector4<T>::Dot( col1, row2 );
        const T& m22 = TVector4<T>::Dot( col2, row2 );
        const T& m32 = TVector4<T>::Dot( col3, row2 );
        
        const T& m03 = TVector4<T>::Dot( col0, row3 );
        const T& m13 = TVector4<T>::Dot( col1, row3 );
        const T& m23 = TVector4<T>::Dot( col2, row3 );
        const T& m33 = TVector4<T>::Dot( col3, row3 );
        
        const TMatrix4x4<T> result
        (
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TVector4<T> TMatrix4x4<T>::Multiply( const TVector4<T>& vector ) const
    {
        TVector4<T> result(
            GetRow( 0 ).Dot( vector ),
            GetRow( 1 ).Dot( vector ),
            GetRow( 2 ).Dot( vector ),
            GetRow( 3 ).Dot( vector )
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TVector4<T> TMatrix4x4<T>::MultiplyTransposed( const TVector4<T>& vector ) const
    {
        TVector4<T> result(
            GetColumn( 0 ).Dot( vector ),
            GetColumn( 1 ).Dot( vector ),
            GetColumn( 2 ).Dot( vector ),
            GetColumn( 3 ).Dot( vector )
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TMatrix4x4<T> TMatrix4x4<T>::MultiplyTransposed( const TMatrix4x4& other ) const
    {
        const TVector4<T> &col0 = GetColumn( 0 ), &col1 = GetColumn( 1 ), &col2 = GetColumn( 2 ), &col3 = GetColumn( 3 );
        const TVector4<T> &row0 = other.GetRow( 0 ), &row1 = other.GetRow( 1 ), &row2 = other.GetRow( 2 ), &row3 = other.GetRow( 3 );

        const T& m00 = TVector4<T>::Dot( col0, row0 );
        const T& m10 = TVector4<T>::Dot( col0, row1 );
        const T& m20 = TVector4<T>::Dot( col0, row2 );
        const T& m30 = TVector4<T>::Dot( col0, row3 );

        const T& m01 = TVector4<T>::Dot( col1, row0 );
        const T& m11 = TVector4<T>::Dot( col1, row1 );
        const T& m21 = TVector4<T>::Dot( col1, row2 );
        const T& m31 = TVector4<T>::Dot( col1, row3 );
        
        const T& m02 = TVector4<T>::Dot( col2, row0 );
        const T& m12 = TVector4<T>::Dot( col2, row1 );
        const T& m22 = TVector4<T>::Dot( col2, row2 );
        const T& m32 = TVector4<T>::Dot( col2, row3 );
        
        const T& m03 = TVector4<T>::Dot( col3, row0 );
        const T& m13 = TVector4<T>::Dot( col3, row1 );
        const T& m23 = TVector4<T>::Dot( col3, row2 );
        const T& m33 = TVector4<T>::Dot( col3, row3 );

        const TMatrix4x4<T> result
        (
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );

        return result;
    }

    template <typename T>
    inline HOST_DEVICE TVector4<T> TMatrix4x4<T>::MultiplyPoint( const TVector3<T>& vector ) const
    {
        TVector4<T> const vect = TVector4<T>( vector, T(1) );
        return MultiplyVector( vect );
    }

    template <typename T>
    inline TVector3<T> TMatrix4x4<T>::MultiplyVector( const TVector3<T>& vector ) const
    {
        TVector4<T> const vect = TVector4<T>( vector, T(0) );
        TVector3<T> result(
            GetRow( 0 ).Dot( vect ),
            GetRow( 1 ).Dot( vect ),
            GetRow( 2 ).Dot( vect )
        );
        return result;
    }

    template <typename T>
    FORCEINLINE TMatrix4x4<T> TMatrix4x4<T>::operator*( const TMatrix4x4& other ) const
    {
        const TVector4<T>& row0 = GetRow( 0 ), & row1 = GetRow( 1 ), & row2 = GetRow( 2 ), & row3 = GetRow( 3 );
        const TVector4<T>& col0 = other.GetColumn( 0 ), & col1 = other.GetColumn( 1 ), & col2 = other.GetColumn( 2 ), & col3 = other.GetColumn( 3 );

        const T& m00 = TVector4<T>::Dot( col0, row0 );
        const T& m10 = TVector4<T>::Dot( col1, row0 );
        const T& m20 = TVector4<T>::Dot( col2, row0 );
        const T& m30 = TVector4<T>::Dot( col3, row0 );

        const T& m01 = TVector4<T>::Dot( col0, row1 );
        const T& m11 = TVector4<T>::Dot( col1, row1 );
        const T& m21 = TVector4<T>::Dot( col2, row1 );
        const T& m31 = TVector4<T>::Dot( col3, row1 );

        const T& m02 = TVector4<T>::Dot( col0, row2 );
        const T& m12 = TVector4<T>::Dot( col1, row2 );
        const T& m22 = TVector4<T>::Dot( col2, row2 );
        const T& m32 = TVector4<T>::Dot( col3, row2 );

        const T& m03 = TVector4<T>::Dot( col0, row3 );
        const T& m13 = TVector4<T>::Dot( col1, row3 );
        const T& m23 = TVector4<T>::Dot( col2, row3 );
        const T& m33 = TVector4<T>::Dot( col3, row3 );

        const TMatrix4x4<T> result
        (
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TVector4<T> TMatrix4x4<T>::operator*( const TVector4<T>& vector ) const
    {
        TVector4<T> result(
            TVector4<T>::Dot( GetRow( 0 ), vector ),
            TVector4<T>::Dot( GetRow( 1 ), vector ),
            TVector4<T>::Dot( GetRow( 2 ), vector ),
            TVector4<T>::Dot( GetRow( 3 ), vector )
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TVector3<T> TMatrix4x4<T>::operator*( const TVector3<T>& vector ) const
    {
        TVector3<T> result(
            TVector3<T>::Dot( GetRow( 0 ), vector ),
            TVector3<T>::Dot( GetRow( 0 ), vector ),
            TVector3<T>::Dot( GetRow( 0 ), vector )
        );

        return result;
    }

    template <typename T>
    FORCEINLINE TMatrix4x4<T> TMatrix4x4<T>::operator*( const T& value ) const
    {
        TMatrix4x4 result( *this );

        result.c0 *= value;
        result.c1 *= value;
        result.c2 *= value;
        result.c3 *= value;

        return result;
    }

    template <typename T>
    FORCEINLINE TMatrix4x4<T> TMatrix4x4<T>::operator/( const T& value ) const
    {
        TMatrix4x4 result( *this );

        result.c0 /= value;
        result.c1 /= value;
        result.c2 /= value;
        result.c3 /= value;

        return result;
    }

    template <typename T>
    inline const T* TMatrix4x4<T>::PointerToValue( void ) const
    {
        return &m00;
    }
}
