
#include "Engine/TaskIcon.h"

#include <Core/Collections.h>

namespace EE
{
    struct WindowsMenuItem
    {
        WString label;
        void* userData;
        MenuItemCallback callback;
        MENUITEMINFO itemInfo;
    };

    struct WindowsTaskIconData
    {
        WNDCLASSEX windowClass;
        HMENU popupMenu;
        HWND mainWindow;
        NOTIFYICONDATA iconData;
        TArray<WindowsMenuItem> menuItems;
    };

    //* Cointains the properties and functions of a task icon
    class WindowsTaskIcon : public TaskIcon
    {
    public:
        WindowsTaskIcon( const TaskIconCreateInfo& createInfo );

        ~WindowsTaskIcon() override;

        //* Rename the icon name
        void SetName( const WString& newName ) override;

        //* Get the icon name
        const WString& GetName() const override;

        void AddMenuItem( const TaskIconMenuItemCreateInfo& menu ) override;

    protected:
        WString name_;
        struct WindowsTaskIconData data_;
    };
}