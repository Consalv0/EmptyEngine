#pragma once

namespace EE
{
    namespace Math
    {
        template <typename T>
        class TTransform
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            TVector3<T> position;
            TQuaternion<T> rotation;
            TVector3<T> scale;

            TTransform();
            TTransform( const TVector3<T>& position, const TQuaternion<T>& rotation, const TVector3<T>& scale );
            TTransform( const TVector3<T>& position, const TQuaternion<T>& rotation );
            TTransform( const TVector3<T>& position );
            TTransform( const TMatrix4x4<T>& matrix );
            TTransform( const TTransform<T>& other );

            TVector3<T> Forward() const;
            TVector3<T> Up() const;
            TVector3<T> Right() const;

            TTransform operator*( const TTransform<T>& other ) const;

            //* Get the inverse of the Model matrix
            TMatrix4x4<T> GetWorldToLocalMatrix() const;

            //* Get the model matrix
            TMatrix4x4<T> GetLocalToWorldMatrix() const;

            //* Get the view matrix
            TMatrix4x4<T> GetWorldToViewMatrix() const;
        };
    }
}

#include "Math/Transform.inl"