
namespace EE
{
    class JoltPhysicsEngine;

    class JoltPhysicsShapeSphere : public PhysicsShapeSphere
    {
    public:
        JPH::Shape* GetJoltShape() { return _shape; };

        JoltPhysicsShapeSphere( const PhysicsShapeSphereCreateInfo& createInfo );
        ~JoltPhysicsShapeSphere( );

    private:
        JPH::Shape* _shape;
    };

    class JoltPhysicsShapeBox : public PhysicsShapeBox
    {
    public:
        JPH::Shape* GetJoltShape() { return _shape; };

        JoltPhysicsShapeBox( const PhysicsShapeBoxCreateInfo& createInfo );
        ~JoltPhysicsShapeBox();

    private:
        JPH::Shape* _shape;
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
        JPH::BodyID _bodyId;
        JPH::PhysicsSystem* _physicsSystem;
        PhysicsShape* _physicsShape;
    };

    class JoltPhysicsEngine : public PhysicsEngine
    {
    public:
        JoltPhysicsEngine();
        ~JoltPhysicsEngine() override;

        void StartSimulation() override;

        bool UpdateSimulation( uint8 steps ) override;

        PhysicsShapeBox* CreateBoxShape( const PhysicsShapeBoxCreateInfo& createInfo ) override;
        PhysicsShapeSphere* CreateSphereShape( const PhysicsShapeSphereCreateInfo& createInfo ) override;

        PhysicsBody* CreateBody( const PhysicsBodyCreateInfo& createInfo ) override;

        FORCEINLINE JPH::PhysicsSystem* GetPhysicsSystem() { return _physicsSystem; }
    
    private:
        JPH::PhysicsSystem* _physicsSystem;
        JPH::BroadPhaseLayerInterface* _broadPhaseLayerInterface;
        JPH::ObjectVsBroadPhaseLayerFilter* _objectVsBroadphaseLayerFilter;
        JPH::ObjectLayerPairFilter* _objectVSObjectLayerFilter;
        JPH::BodyActivationListener* _bodyActivationListener;
        JPH::ContactListener* _contactListener;
    };
}