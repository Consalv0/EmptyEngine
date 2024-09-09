
namespace EE
{
    class JoltPhysicsEngine;

    class JoltPhysicsShapeSphere : public PhysicsShapeSphere
    {
    public:
        JPH::Shape* GetJoltShape() { return shape_; };

        JoltPhysicsShapeSphere( const PhysicsShapeSphereCreateInfo& createInfo );
        ~JoltPhysicsShapeSphere( );

    private:
        JPH::Shape* shape_;
    };

    class JoltPhysicsShapeBox : public PhysicsShapeBox
    {
    public:
        JPH::Shape* GetJoltShape() { return shape_; };

        JoltPhysicsShapeBox( const PhysicsShapeBoxCreateInfo& createInfo );
        ~JoltPhysicsShapeBox();

    private:
        JPH::Shape* shape_;
    };

    class JoltPhysicsBody : public PhysicsBody
    {
        EE_CLASSNOCOPY( JoltPhysicsBody )

    public:
        JoltPhysicsBody( const PhysicsBodyCreateInfo& createInfo, JoltPhysicsEngine* physicsEngine );

        virtual ~JoltPhysicsBody() override;

        void GetPosition( Vector3f* position ) const override;
        void SetPosition( const Vector3f& position ) override;
        void GetRotation( Quaternionf* position ) const override;
        void SetRotation( const Quaternionf& position ) override;
        void GetLinearVelocity( Vector3f* velocity ) const override;
        void SetLinearVelocity( const Vector3f& velocity ) override;
        void GetFriction( float* friction ) const override;
        void SetFriction( const float& friction ) override;

    private:
        JPH::BodyID bodyId_;
        JPH::PhysicsSystem* physicsSystem_;
        PhysicsShape* physicsShape_;
    };

    class JoltPhysicsEngine : public PhysicsEngine
    {
    public:
        JoltPhysicsEngine();
        ~JoltPhysicsEngine() override;

        void StartSimulation();

        bool UpdateSimulation( uint8 steps );

        PhysicsShapeBox* CreateBoxShape( const PhysicsShapeBoxCreateInfo& createInfo ) override;
        PhysicsShapeSphere* CreateSphereShape( const PhysicsShapeSphereCreateInfo& createInfo ) override;

        PhysicsBody* CreateBody( const PhysicsBodyCreateInfo& createInfo ) override;

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