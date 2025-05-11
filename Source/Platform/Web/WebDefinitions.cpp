
#include "CoreMinimal.h"

#include <Engine/Engine.h>
#include <Engine/Input.h>
#include "Platform/PrePlatform.h"
#include <Platform/Platform.h>
#include "Platform/PostPlatform.h"
#include <Engine/TaskIcon.h>

#include <Core/Log.h>

namespace EE
{
    void GameEngine::ShowConsole( bool show )
    {
        if ( show == false )
        {
            Log::Shotdown();
        }
        else
        {
            Log::Initialize();
        }
    }

    class DummyTaskIcon : public TaskIcon
    {
    public:
        DummyTaskIcon( const TaskIconCreateInfo& createInfo ) : name_( createInfo.name ) {}

        ~DummyTaskIcon() override {};
        //* Rename the icon name
        void SetName( const U8String& newName ) override { name_ = newName; };

        //* Get the icon name
        const U8String& GetName() const override { return name_; }

        void AddMenuItem( const TaskIconMenuItemCreateInfo& menu ) override {};

    protected:
        U8String name_;
    };

    TaskIcon* PlatformCreateTaskIcon( const TaskIconCreateInfo& info )
    {
        return new DummyTaskIcon( info );
    }

    Input* GInput = NULL;

    Input* PlatformCreateInput()
    {
        return new Input();
    }
}