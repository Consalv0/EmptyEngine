#pragma once

namespace EE
{
    namespace Math
    {
        template<typename T>
        struct TBox2
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T left, bottom, right, top; };
                struct { T minX, minY, maxX, maxY; };
            };

            constexpr TBox2() = default;

            constexpr TBox2( const T& minX, const T& minY, const T& maxX, const T& maxY )
                : minX( minX ), minY( minY ), maxX( maxX ), maxY( maxY )
            {
            }

            constexpr TBox2( const T& minX, const T& minY, const TVector2<T>& max )
                : minX( minX ), minY( minY ), maxX( max.x ), maxY( max.y )
            {
            }

            constexpr TBox2( const TVector2<T>& min, const TVector2<T>& max )
                : minX( min.x ), minY( min.y ), maxX( max.x ), maxY( max.y )
            {
            }

            //* Get the lower point of the bounding box
            FORCEINLINE TVector2<T> GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ) }; }

            //* Get the upper point of the bounding box
            FORCEINLINE TVector2<T> GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ) }; }

            //* Get the width of the bounding box
            FORCEINLINE T GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

            //* Get the height of the bounding box
            FORCEINLINE T GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

            //* Get the area of the bounding box
            FORCEINLINE T GetArea() const { return GetWidth() * GetHeight(); }

            //* Get the perimeter of the bounding box
            FORCEINLINE T GetPerimeter() const { return GetWidth() * T( 2 ) + GetHeight() * T( 2 ); }

            //* Increase bounds by adding a point
            inline void AddPoint( const TVector2<T>& point )
            {
                minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y );
                maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y );
            }

            //* Increase bounds by adding a point
            inline void AddPoint( const TVector3<T>& point )
            {
                minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y );
                maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y );
            }

            //* Increase bounds by adding a point
            inline void AddPoint( const TVector4<T>& point )
            {
                minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y );
                maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y );
            }
        };
    }
}