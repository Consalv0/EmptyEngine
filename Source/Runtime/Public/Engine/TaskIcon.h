#pragma once

namespace EE
{
    typedef std::function<void( void* userData )> MenuItemCallback;

    struct TaskIconCreateInfo
    {
        WString name = L"Empty Engine";
    };

    struct TaskIconMenuItemCreateInfo
    {
        WString label = L"Label";
        void* userData;
        MenuItemCallback callback;
    };

    //* Cointains the properties and functions of a task icon
    class TaskIcon
    {
    public:
        virtual ~TaskIcon() {};

        //* Rename the icon name
        virtual void SetName( const WString& newName ) = 0;

        virtual void AddMenuItem( const TaskIconMenuItemCreateInfo& menu ) = 0;

        //* Get the icon name
        virtual const WString& GetName() const = 0;
    };

    //* Creates a task menu icon
    TaskIcon* PlatformCreateTaskIcon( const TaskIconCreateInfo& info );
}
