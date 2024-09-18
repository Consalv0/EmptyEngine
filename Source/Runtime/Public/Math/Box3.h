#pragma once

namespace EE
{
    namespace Math
    {
        template<typename T>
        struct TBox3
        {
            static_assert(std::is_floating_point_v<T>, "T must be floating point.");

        public:
            union
            {
                struct { T left, bottom, back, right, top, front; };
                struct { T minX, minY, minZ, maxX, maxY, maxZ; };
            };

            constexpr TBox3()
            {
                minX = minY = minZ = MathConstants<T>::MinValue;
                maxX = maxY = maxZ = MathConstants<T>::MaxValue;
            }

            constexpr TBox3( T minX, T minY, T minZ, T maxX, T maxY, T maxZ )
                : minX( minX ), minY( minY ), minZ( minZ ), maxX( maxX ), maxY( maxY ), maxZ( maxZ )
            {
            }

            inline TBox3<T> Transform( const TMatrix4x4<T>& transformation ) const
            {
                TBox3<T> value;
                // Min-max N
                value.Add( transformation.MultiplyPoint( TVector3<T>( minX, minY, minZ ) ) );
                value.Add( transformation.MultiplyPoint( TVector3<T>( maxX, maxY, maxZ ) ) );
                // Min-max x
                value.Add( transformation.MultiplyPoint( TVector3<T>( maxX, minY, minZ ) ) );
                value.Add( transformation.MultiplyPoint( TVector3<T>( minX, maxY, maxZ ) ) );
                // Min-max y
                value.Add( transformation.MultiplyPoint( TVector3<T>( minX, maxY, minZ ) ) );
                value.Add( transformation.MultiplyPoint( TVector3<T>( maxX, minY, maxZ ) ) );
                // Min-max z
                value.Add( transformation.MultiplyPoint( TVector3<T>( minX, minY, maxZ ) ) );
                value.Add( transformation.MultiplyPoint( TVector3<T>( maxX, maxY, minZ ) ) );
                return value;
            }

            //* Add point to the BondingBox
            inline void Add( TVector3<T> point )
            {
                minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y ); minZ = Math::Min( minZ, point.z );
                maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y ); maxZ = Math::Max( maxZ, point.z );
            };

            //* Get the dimensions of the bounding box
            FORCEINLINE TVector3<T> GetSize() const { return TVector3<T>( GetWidth(), GetHeight(), GetDepth() ); }

            //* Get the center position of the bounding box
            FORCEINLINE TVector3<T> GetCenter() const { return TVector3<T>( minX + maxX, minY + maxY, minZ + maxZ ) * T( 0.5 ); }

            //* Get the lower point of the bounding box
            FORCEINLINE TVector3<T> GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ), Math::Min( front, back ) }; }

            //* Get the upper point of the bounding box
            FORCEINLINE TVector3<T> GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ), Math::Max( front, back ) }; }

            //* Get the width of the bounding box
            FORCEINLINE T GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

            //* Get the height of the bounding box
            FORCEINLINE T GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

            //* Get the depth of the bounding box
            FORCEINLINE T GetDepth() const { return Math::Max( front, back ) - Math::Min( front, back ); }

            //* Get the area of the bounding box
            FORCEINLINE T GetArea() const { return GetWidth() * GetHeight() * GetDepth(); }

            //* Get the perimeter of the bounding box
            FORCEINLINE T GetPerimeter() const { return GetWidth() * T( 2 ) + GetHeight() * T( 2 ) + GetDepth() * T( 2 ); }

            // Used in frustrum computations
            inline TVector3<T> GetPointPositive( const TVector3<T>& normal ) const
            {
                TVector3<T> maxPoint = GetMaxPoint();
                TVector3<T> result = GetMinPoint();
                if ( normal.x > 0 ) result.x = maxPoint.x;
                if ( normal.y > 0 ) result.y = maxPoint.y;
                if ( normal.z > 0 ) result.z = maxPoint.z;
                return result;
            }

            // Used in frustrum computations
            inline TVector3<T> GetPointNegative( const TVector3<T>& normal ) const
            {
                TVector3<T> maxPoint = GetMaxPoint();
                TVector3<T> result = GetMinPoint();
                if ( normal.x < 0 ) result.x = maxPoint.x;
                if ( normal.y < 0 ) result.y = maxPoint.y;
                if ( normal.z < 0 ) result.z = maxPoint.z;
                return result;
            }
        };
    }
}