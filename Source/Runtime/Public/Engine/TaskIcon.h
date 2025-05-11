#pragma once

namespace EE
{
    typedef std::function<void( void* userData )> MenuItemCallback;

    struct TaskIconCreateInfo
    {
        U8String name = "Empty Engine";
    };

    struct TaskIconMenuItemCreateInfo
    {
        U8String label = "Label";
        void* userData;
        MenuItemCallback callback;
    };

    //* Cointains the properties and functions of a task icon
    class TaskIcon
    {
    public:
        virtual ~TaskIcon() {};

        //* Rename the icon name
        virtual void SetName( const U8String& newName ) = 0;

        virtual void AddMenuItem( const TaskIconMenuItemCreateInfo& menu ) = 0;

        //* Get the icon name
        virtual const U8String& GetName() const = 0;
    };

    //* Creates a task menu icon
    TaskIcon* PlatformCreateTaskIcon( const TaskIconCreateInfo& info );
}
