
#include <CoreMinimal.h>

#include <Core/Log.h>
#include <Math/CoreMath.h>
#include <Utils/TextFormatting.h>
#include <Utils/VariableWatcher.h>

#include <Physics/PhysicsEngine.h>
#include <Physics/JoltPhysics.h>
#include <Physics/JoltPhysicsEngine.h>

namespace EE
{
    PhysicsEngine* GPhysicsEngine = NULL;

    PhysicsEngine* CreatePhysicsEngine()
    {
        return new JoltPhysicsEngine();
    }
}