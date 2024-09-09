
namespace EE
{
    class JoltPhysicsEngine;

    class JoltSphereBody : public PhysicsSphereBody
    {
        EE_CLASSNOCOPY( JoltSphereBody )

    public:
        JoltSphereBody( const PhysicsSphereBodyCreateInfo& createInfo, JoltPhysicsEngine* physicsEngine );

        virtual ~JoltSphereBody() override;

        Vector3f GetPosition() const;
        void SetPosition( const Vector3f& position );
        Vector3f GetLinearVelocity() const;
        void SetLinearVelocity( const Vector3f& velocity );

    private:
        JPH::BodyID bodyId_;
        JPH::PhysicsSystem* physicsSystem_;
    };

    class JoltBoxBody : public PhysicsBoxBody
    {
        EE_CLASSNOCOPY( JoltBoxBody )

    public:
        JoltBoxBody( const PhysicsBoxBodyCreateInfo& createInfo, JoltPhysicsEngine* physicsEngine );

        virtual ~JoltBoxBody() override;

        Vector3f GetPosition() const;
        void SetPosition( const Vector3f& position );
        Vector3f GetLinearVelocity() const;
        void SetLinearVelocity( const Vector3f& velocity );

    private:
        JPH::BodyID bodyId_;
        JPH::PhysicsSystem* physicsSystem_;
    };

    class JoltPhysicsEngine : public PhysicsEngine
    {
    public:
        JoltPhysicsEngine();
        ~JoltPhysicsEngine() override;

        void StartSimulation();

        bool UpdateSimulation( uint8 steps );

        PhysicsSphereBody* CreateSphereBody( const PhysicsSphereBodyCreateInfo& createInfo );

        PhysicsBoxBody* CreateBoxBody( const PhysicsBoxBodyCreateInfo& createInfo );

        FORCEINLINE JPH::PhysicsSystem* GetPhysicsSystem() { return physicsSystem_; }
    
    private:
        JPH::PhysicsSystem* physicsSystem_;
        JPH::BroadPhaseLayerInterface* broadPhaseLayerInterface_;
        JPH::ObjectVsBroadPhaseLayerFilter* objectVsBroadphaseLayerFilter_;
        JPH::ObjectLayerPairFilter* objectVSObjectLayerFilter_;
        JPH::BodyActivationListener* bodyActivationListener_;
        JPH::ContactListener* contactListener_;
    };
}