#ifndef EE_PHYSICS_ENGINE
#define EE_PHYSICS_ENGINE
#pragma once

#include <Math/CoreMath.h>

namespace EE
{
    enum EPhysicsShape
    {
        PhysicsShape_Undefined,
        PhysicsShape_Box,
        PhysicsShape_Sphere,
    };

    enum EMotionType
    {
        MotionTye_Static,     ///< Non movable
        MotionTye_Kinematic,  ///< Movable using velocities only, does not respond to forces
        MotionTye_Dynamic,    ///< Responds to forces as a normal physics object
    };

    class PhysicsShape
    {
        EE_CLASSNOCOPY( PhysicsShape )

    protected:
        PhysicsShape() = delete;

        PhysicsShape( EPhysicsShape shape ) : shape( shape ) {};

    public:
        const EPhysicsShape shape = PhysicsShape_Undefined;
    };

    struct PhysicsShapeSphereCreateInfo
    {
        float radius = 0.0F;
    };

    class PhysicsShapeSphere : public PhysicsShape
    {
    protected:
        PhysicsShapeSphere( const PhysicsShapeSphereCreateInfo& createInfo ) : PhysicsShape( PhysicsShape_Sphere )
            , radius_( createInfo.radius )
        {
        }

    protected:
        const float radius_;
    };

    struct PhysicsShapeBoxCreateInfo
    {
        Vector3f extents = Vector3f();
    };

    class PhysicsShapeBox : public PhysicsShape
    {
    protected:
        PhysicsShapeBox( const PhysicsShapeBoxCreateInfo& createInfo ) : PhysicsShape( PhysicsShape_Box )
            , extents_( createInfo.extents )
        {
        }

    protected:
        const Vector3f extents_;
    };

    struct PhysicsBodyCreateInfo
    {
        PhysicsShape* physicsShape = NULL;
        Vector3f position = Vector3f();
        Quaternionf rotation = Quaternionf();
        Vector3f velocity = Vector3f();
        EMotionType motionType = MotionTye_Static;
        bool activate = true;
    };

    class PhysicsBody
    {
        EE_CLASSNOCOPY( PhysicsBody )

    protected:
        PhysicsBody() {};

    public:
        virtual ~PhysicsBody() {};

        virtual void GetPosition( Vector3f* position ) const = 0;
        virtual void SetPosition( const Vector3f& position ) = 0;
        virtual void GetRotation( Quaternionf* position ) const = 0;
        virtual void SetRotation( const Quaternionf& position ) = 0;
        virtual void GetLinearVelocity( Vector3f* position ) const = 0;
        virtual void SetLinearVelocity( const Vector3f& velocity ) = 0;
        virtual void GetFriction( float* friction ) const = 0;
        virtual void SetFriction( const float& friction ) = 0;
    };

    class PhysicsEngine
    {
        EE_CLASSNOCOPY( PhysicsEngine )

    protected:
        PhysicsEngine() {};

    public:
        virtual ~PhysicsEngine() {};

        virtual void StartSimulation() = 0;

        virtual bool UpdateSimulation( uint8 steps ) = 0;

        virtual PhysicsShapeBox* CreateBoxShape( const PhysicsShapeBoxCreateInfo& createInfo ) = 0;
        virtual PhysicsShapeSphere* CreateSphereShape( const PhysicsShapeSphereCreateInfo& createInfo ) = 0;
        
        virtual PhysicsBody* CreateBody( const PhysicsBodyCreateInfo& createInfo ) = 0;
    };

    extern PhysicsEngine* GPhysicsEngine;

    PhysicsEngine* CreatePhysicsEngine();
}

#endif // EE_PHYSICS_ENGINE