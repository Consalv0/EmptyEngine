
#include <CoreMinimal.h>

#include <Core/Log.h>
#include <Math/CoreMath.h>
#include <Utils/TextFormatting.h>

#include <Physics/PhysicsEngine.h>
#include <Physics/JoltPhysics.h>
#include <Physics/JoltPhysicsEngine.h>

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

namespace EE
{
    static JPH::EMotionType ConvertMotionType( EMotionType type )
    {
        switch ( type )
        {
        case EMotionType::MotionTye_Static:     return JPH::EMotionType::Static;
        case EMotionType::MotionTye_Kinematic:  return JPH::EMotionType::Kinematic;
        case EMotionType::MotionTye_Dynamic:    return JPH::EMotionType::Dynamic;
        default:
            return JPH::EMotionType::Static;
        }
    }

    // Callback for traces, connect this to your own trace function if you have one
    static void CallbackTrace( const char* inFMT, ... )
    {
        // Format the message
        va_list list;
        va_start( list, inFMT );
        char buffer[ 1'024 ];
        vsnprintf( buffer, sizeof( buffer ), inFMT, list );
        va_end( list );

        // Print to the TTY
        EE_LOG_INFO( L"[Jolt] {}", Text::NarrowToWide( buffer ) );
    }

#ifdef JPH_ENABLE_ASSERTS
    // Callback for asserts, connect this to your own assert handler if you have one
    static bool CallbackAssertFailed( const char* inExpression,
        const char* inMessage,
        const char* inFile,
        uint32 inLine )
    {
        EE_LOG_CRITICAL( L"[Jolt] Assertion Failed {} : {} : ({}) ", Text::NarrowToWide( inFile ), inLine, Text::NarrowToWide( inExpression ), (inMessage != NULL ? Text::NarrowToWide( inMessage ) : L"") );

        // Breakpoint
        return true;
    };

#endif // JPH_ENABLE_ASSERTS

    // Layer that objects can be in, determines which other objects it can collide with
    // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
    // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
    // but only if you do collision testing).
    namespace Layers
    {
        static constexpr JPH::ObjectLayer NonMoving = 0;
        static constexpr JPH::ObjectLayer Moving = 1;
        static constexpr JPH::ObjectLayer Max = 2;
    };

    /// Class that determines if two object layers can collide
    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override
        {
            switch ( inObject1 )
            {
            case Layers::NonMoving:
                return inObject2 == Layers::Moving; // Non moving only collides with moving
            case Layers::Moving:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT( false );
                return false;
            }
        }
    };

    // Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
    // a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
    // You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
    // many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
    // your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NonMoving( 0 );
        static constexpr JPH::BroadPhaseLayer Moving( 1 );
        static constexpr JPH::uint Max( 2 );
    };

    // BroadPhaseLayerInterface implementation
    // This defines a mapping between object and broadphase layers.
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl()
        {
            // Create a mapping table from object to broad phase layer
            mObjectToBroadPhase[ Layers::NonMoving ] = BroadPhaseLayers::NonMoving;
            mObjectToBroadPhase[ Layers::Moving ] = BroadPhaseLayers::Moving;
        }

        virtual uint32 GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::Max;
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override
        {
            JPH_ASSERT( inLayer < Layers::Max );
            return mObjectToBroadPhase[ inLayer ];
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const NChar* GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const override
        {
            switch ( (JPH::BroadPhaseLayer::Type)inLayer )
            {
            case (JPH::BroadPhaseLayer::Type)JPH::BroadPhaseLayers::NonMoving: return "NonMoving";
            case (JPH::BroadPhaseLayer::Type)JPH::BroadPhaseLayers::Moving:    return "Moving";
            default: JPH_ASSERT( false ); return "Invalid";
            }
        }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[ Layers::Max ];
    };

    /// Class that determines if an object layer can collide with a broadphase layer
    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
        {
            switch ( inLayer1 )
            {
            case Layers::NonMoving:
                return inLayer2 == BroadPhaseLayers::Moving;
            case Layers::Moving:
                return true;
            default:
                JPH_ASSERT( false );
                return false;
            }
        }
    };

    // An example contact listener
    class MyContactListener : public JPH::ContactListener
    {
    public:
        // See: ContactListener
        virtual JPH::ValidateResult OnContactValidate( const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult ) override
        {
            EE_LOG_INFO( L"Contact validate callback" );

            // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        virtual void OnContactAdded( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
        {
            EE_LOG_INFO( L"A contact was added" );
        }

        virtual void OnContactPersisted( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
        {
            EE_LOG_INFO( L"A contact was persisted" );
        }

        virtual void OnContactRemoved( const JPH::SubShapeIDPair& inSubShapePair ) override
        {
            EE_LOG_INFO( L"A contact was removed" );
        }
    };

    // An example activation listener
    class MyBodyActivationListener : public JPH::BodyActivationListener
    {
    public:
        virtual void OnBodyActivated( const JPH::BodyID& inBodyID, uint64 inBodyUserData ) override
        {
            EE_LOG_INFO( L"A body got activated" );
        }

        virtual void OnBodyDeactivated( const JPH::BodyID& inBodyID, uint64 inBodyUserData ) override
        {
            EE_LOG_INFO( L"A body went to sleep" );
        }
    };

    JoltPhysicsShapeSphere::JoltPhysicsShapeSphere( const PhysicsShapeSphereCreateInfo& createInfo ) : PhysicsShapeSphere( createInfo )
        , shape_( NULL )
    {
        shape_ = new JPH::SphereShape( radius_ );
    }

    JoltPhysicsShapeSphere::~JoltPhysicsShapeSphere()
    {
        delete shape_;
    }

    JoltPhysicsShapeBox::JoltPhysicsShapeBox( const PhysicsShapeBoxCreateInfo& createInfo ) : PhysicsShapeBox( createInfo )
        , shape_( NULL )
    {
        shape_ = new JPH::BoxShape( JPH::Vec3( extents_.x, extents_.y, extents_.z ) );
    }

    JoltPhysicsShapeBox::~JoltPhysicsShapeBox()
    {
        delete shape_;
    }

    JoltPhysicsEngine::JoltPhysicsEngine()
    {
        // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
        // This needs to be done before any other Jolt function is called.
        JPH::RegisterDefaultAllocator();

        // Install trace and assert callbacks
        JPH::Trace = CallbackTrace;
        JPH_IF_ENABLE_ASSERTS( JPH::AssertFailed = CallbackAssertFailed );

        // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
        // It is not directly used in this example but still required.
        JPH::Factory::sInstance = new JPH::Factory();

        // Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
        // If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
        // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
        JPH::RegisterTypes();

        // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
        const uint32 cMaxBodies = 1024;

        // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
        const uint32 cNumBodyMutexes = 0;

        // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
        // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
        // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
        const uint32 cMaxBodyPairs = 1024;

        // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
        // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
        const uint32 cMaxContactConstraints = 1024;

        // Create mapping table from object layer to broadphase layer
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        broadPhaseLayerInterface_ = new BPLayerInterfaceImpl();

        // Create class that filters object vs broadphase layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        objectVsBroadphaseLayerFilter_ = new ObjectVsBroadPhaseLayerFilterImpl();

        // Create class that filters object vs object layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        objectVSObjectLayerFilter_ = new ObjectLayerPairFilterImpl();

        // Now we can create the actual physics system.
        physicsSystem_ = new JPH::PhysicsSystem();
        physicsSystem_->Init( cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broadPhaseLayerInterface_, *objectVsBroadphaseLayerFilter_, *objectVSObjectLayerFilter_ );

        // A body activation listener gets notified when bodies activate and go to sleep
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        bodyActivationListener_ = new MyBodyActivationListener();
        physicsSystem_->SetBodyActivationListener( bodyActivationListener_ );

        // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        contactListener_ = new MyContactListener();
        physicsSystem_->SetContactListener( contactListener_ );
    }

    JoltPhysicsEngine::~JoltPhysicsEngine()
    {
        // Unregisters all types with the factory and cleans up the default material
        JPH::UnregisterTypes();

        // Destroy the factory
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = NULL;

        delete physicsSystem_;
        delete bodyActivationListener_;
        delete contactListener_;
        delete broadPhaseLayerInterface_;
        delete objectVsBroadphaseLayerFilter_;
        delete objectVSObjectLayerFilter_;
    }

    void JoltPhysicsEngine::StartSimulation()
    {
        // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
        // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
        // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
        physicsSystem_->OptimizeBroadPhase();
    }

    bool JoltPhysicsEngine::UpdateSimulation( uint8 steps )
    {
        if ( steps == 0 )
            return false;

        steps = Math::Min( steps, (uint8)3u );

        // We need a temp allocator for temporary allocations during the physics update. We're
        // pre-allocating 10 MB to avoid having to do allocations during the physics update.
        // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
        // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
        // malloc / free.
        static JPH::TempAllocatorImpl tempAllocator( 10 * 1024 * 1024 );

        // We need a job system that will execute physics jobs on multiple threads. Typically
        // you would implement the JobSystem interface yourself and let Jolt Physics run on top
        // of your own job scheduler. JobSystemThreadPool is an example implementation.
        static JPH::JobSystemThreadPool jobSystem( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );

        // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
        const float cDeltaTime = 1.0F / 60.0F;

        // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
        int cCollisionSteps = steps;

        // Step the world
        JPH::EPhysicsUpdateError error = physicsSystem_->Update( cDeltaTime, cCollisionSteps, &tempAllocator, &jobSystem );

        if ( error != JPH::EPhysicsUpdateError::None )
        {
            return false;
        }

        return true;
    }

    PhysicsShapeBox* JoltPhysicsEngine::CreateBoxShape( const PhysicsShapeBoxCreateInfo& createInfo )
    {
        return new JoltPhysicsShapeBox( createInfo );
    }

    PhysicsShapeSphere* JoltPhysicsEngine::CreateSphereShape( const PhysicsShapeSphereCreateInfo& createInfo )
    {
        return new JoltPhysicsShapeSphere( createInfo );
    }

    PhysicsBody* JoltPhysicsEngine::CreateBody( const PhysicsBodyCreateInfo& createInfo )
    {
        return new JoltPhysicsBody( createInfo, this );
    }

    JoltPhysicsBody::JoltPhysicsBody( const PhysicsBodyCreateInfo& createInfo, JoltPhysicsEngine* physicsEngine )
        : physicsSystem_( physicsEngine->GetPhysicsSystem() )
        , physicsShape_( createInfo.physicsShape )
        , bodyId_()
    {
        // The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
        // variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();

        JPH::Shape* shape = NULL;
        switch ( physicsShape_->shape )
        {
        case PhysicsShape_Sphere:
        {
            JoltPhysicsShapeSphere* sphereShape = static_cast<JoltPhysicsShapeSphere*>(physicsShape_);
            shape = sphereShape->GetJoltShape();
        }
            break;
        case PhysicsShape_Box:
        {
            JoltPhysicsShapeBox* boxShape = static_cast<JoltPhysicsShapeBox*>(physicsShape_);
            shape = boxShape->GetJoltShape();
        }
            break;
        default:
            break;
        }

        // Now create a dynamic body to bounce on the floor
        // Note that this uses the shorthand version of creating and adding a body to the world
        JPH::BodyCreationSettings sphereSettings
        (
            shape,
            JPH::RVec3( createInfo.position.x, createInfo.position.y, createInfo.position.z ),
            JPH::Quat( createInfo.rotation.x, createInfo.rotation.y, createInfo.rotation.z, createInfo.rotation.w ),
            ConvertMotionType( createInfo.motionType ),
            Layers::Moving
        );

        bodyId_ = bodyInterface.CreateAndAddBody( sphereSettings, createInfo.activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate );
    }

    JoltPhysicsBody::~JoltPhysicsBody()
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();

        // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
        bodyInterface.RemoveBody( bodyId_ );

        // Destroy the sphere. After this the sphere ID is no longer valid.
        bodyInterface.DestroyBody( bodyId_ );
    }

    void JoltPhysicsBody::GetPosition( Vector3f* outPosition ) const
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        JPH::RVec3 position = bodyInterface.GetPosition( bodyId_ );

        outPosition->x = position.GetX();
        outPosition->y = position.GetY();
        outPosition->z = position.GetZ();
    }

    void JoltPhysicsBody::SetPosition( const Vector3f& position )
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        bodyInterface.SetPosition( bodyId_, JPH::RVec3( position.x, position.y, position.z ), JPH::EActivation::DontActivate );
    }

    void JoltPhysicsBody::GetRotation( Quaternionf* outRotation ) const
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        JPH::Quat rotation = bodyInterface.GetRotation( bodyId_ );

        outRotation->x = rotation.GetX();
        outRotation->y = rotation.GetY();
        outRotation->z = rotation.GetZ();
        outRotation->w = rotation.GetW();
    }

    void JoltPhysicsBody::SetRotation( const Quaternionf& rotation )
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        bodyInterface.SetRotation( bodyId_, JPH::Quat( rotation.x, rotation.y, rotation.z, rotation.w ), JPH::EActivation::DontActivate );
    }

    void JoltPhysicsBody::GetLinearVelocity( Vector3f* outVelocity ) const
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        JPH::Vec3 velocity = bodyInterface.GetLinearVelocity( bodyId_ );

        outVelocity->x = velocity.GetX();
        outVelocity->y = velocity.GetY();
        outVelocity->z = velocity.GetZ();
    }

    void JoltPhysicsBody::SetLinearVelocity( const Vector3f& velocity )
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        bodyInterface.SetLinearVelocity( bodyId_, JPH::Vec3( velocity.x, velocity.y, velocity.z ) );
        bodyInterface.ActivateBody( bodyId_ );
    }

    void JoltPhysicsBody::GetFriction( float* friction ) const
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        *friction = bodyInterface.GetFriction( bodyId_ );
    }

    void JoltPhysicsBody::SetFriction( const float& friction )
    {
        JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
        bodyInterface.SetFriction( bodyId_, friction );
    }
}