#ifndef EE_JOLT_USAGE
#define EE_JOLT_USAGE

#if defined( EE_ENABLE_ASSERTS )
#define JPH_ENABLE_ASSERTS
#endif

#if defined( EE_RELEASE )
#define JPH_NO_DEBUG
#endif

#define JPH_CROSS_PLATFORM_DETERMINISTIC

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#endif // EE_JOLT_USAGE