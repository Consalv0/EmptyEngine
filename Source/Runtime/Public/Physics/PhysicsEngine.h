#ifndef EE_PHYSICS_ENGINE
#define EE_PHYSICS_ENGINE
#pragma once

#include <Math/CoreMath.h>

namespace EE
{
    enum EMotionType
    {
        MotionTye_Static,     ///< Non movable
        MotionTye_Kinematic,  ///< Movable using velocities only, does not respond to forces
        MotionTye_Dynamic,    ///< Responds to forces as a normal physics object
    };

    class PhysicsBody
    {
        EE_CLASSNOCOPY( PhysicsBody )

    protected:
        PhysicsBody() {};

    public:
        virtual ~PhysicsBody() {};

        virtual Vector3f GetPosition() const = 0;
        virtual void SetPosition( const Vector3f& position ) = 0;
        virtual Vector3f GetLinearVelocity() const = 0;
        virtual void SetLinearVelocity( const Vector3f& velocity ) = 0;
    };

    struct PhysicsSphereBodyCreateInfo
    {
        float radius;
        Vector3f position;
        Quaternionf rotation;
        Vector3f velocity;
        EMotionType motionType;
        bool activate;
    };

    class PhysicsSphereBody : public PhysicsBody
    {
        EE_CLASSNOCOPY( PhysicsSphereBody )

    protected:
        PhysicsSphereBody() {};

    public:
        virtual ~PhysicsSphereBody() override {};
    };

    struct PhysicsBoxBodyCreateInfo
    {
        Vector3f extents;
        Vector3f position;
        Quaternionf rotation;
        Vector3f velocity;
        EMotionType motionType;
        bool activate;
    };

    class PhysicsBoxBody : public PhysicsBody
    {
        EE_CLASSNOCOPY( PhysicsBoxBody )

    protected:
        // Use GPhysicsEngine->CreateBoxBody
        PhysicsBoxBody() {};

    public:
        virtual ~PhysicsBoxBody() override {};
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

        virtual PhysicsSphereBody* CreateSphereBody( const PhysicsSphereBodyCreateInfo& createInfo ) = 0;

        virtual PhysicsBoxBody* CreateBoxBody( const PhysicsBoxBodyCreateInfo& createInfo ) = 0;
    };

    extern PhysicsEngine* GPhysicsEngine;

    PhysicsEngine* CreatePhysicsEngine();
}

#endif // EE_PHYSICS_ENGINE