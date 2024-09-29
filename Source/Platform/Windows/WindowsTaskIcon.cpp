
#include <CoreMinimal.h>

#include "Platform/Platform.h"

#include <cstring>
#include <dwmapi.h>
#include <windowsx.h>
#include <shellapi.h>

#include "Platform/Windows/WindowsTaskIcon.h"

#define WM_ICON 0x1C0A
#define WM_OURICON 0x1C0B
#define EXIT_ID 0x99

namespace EE
{
    const WChar CLASS_NAME[] = L"EE_TRAY_ICON_CLASS";

    LRESULT CALLBACK WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        WindowsTaskIconData* data = reinterpret_cast<WindowsTaskIconData*>(GetWindowLongPtr( hwnd, GWLP_USERDATA ));
        POINT pt;

        switch ( uMsg )
        {
        case WM_OURICON:
            if ( LOWORD( lParam ) == WM_RBUTTONUP )
            {
                SetForegroundWindow( hwnd );
                GetCursorPos( &pt );
                TrackPopupMenuEx( data->popupMenu, \
                    (GetSystemMetrics( SM_MENUDROPALIGNMENT ) ? TPM_RIGHTALIGN : TPM_LEFTALIGN) | TPM_BOTTOMALIGN, \
                    pt.x, pt.y, hwnd, NULL );
            }
            break;
        case WM_ICON:
            if ( LOWORD( lParam ) == WM_LBUTTONDBLCLK )
            {
                EE_LOG_INFO( L"Tray pressed" );
                // showWindow( context, lParam );
            }
            break;
        case WM_COMMAND:
            if ( HIWORD( wParam ) == 0 )
            {
                WORD id = WORD(0x99) - LOWORD( wParam ) - 1;
                WindowsMenuItem& item = data->menuItems[ id ];
                item.callback( item.userData );
            }
            break;
        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
        return 0;
    }

    WindowsTaskIcon::WindowsTaskIcon( const TaskIconCreateInfo& createInfo )
        : name_( createInfo.name )
        , data_( )
    {
        HINSTANCE hInstance = GetModuleHandle( NULL );

        data_.windowClass.cbSize = sizeof( WNDCLASSEX );
        data_.windowClass.style = CS_GLOBALCLASS;
        data_.windowClass.lpfnWndProc = WndProc;
        data_.windowClass.cbClsExtra = 0;
        data_.windowClass.cbWndExtra = 0;
        data_.windowClass.hInstance = hInstance;
        data_.windowClass.hIcon = nullptr;
        data_.windowClass.hIconSm = nullptr;
        data_.windowClass.hCursor = nullptr;
        data_.windowClass.hbrBackground = nullptr;
        data_.windowClass.lpszMenuName = nullptr;
        data_.windowClass.lpszClassName = CLASS_NAME;

        if ( RegisterClassEx( &data_.windowClass ) )
            data_.mainWindow = CreateWindowEx( 0, CLASS_NAME, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL );

        SetWindowLongPtr( data_.mainWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&data_) );

        data_.iconData.cbSize = sizeof( NOTIFYICONDATA );
        data_.iconData.hWnd = data_.mainWindow;
        data_.iconData.hIcon = static_cast<HICON>(LoadImage( hInstance, L"MAINICON", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE ));
        data_.iconData.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE;
        data_.iconData.uVersion = NOTIFYICON_VERSION_4;
        data_.iconData.uID = (UINT)(DWORD_PTR)data_.mainWindow;
        data_.iconData.uCallbackMessage = WM_OURICON;
        wcscpy_s( data_.iconData.szTip, name_.c_str() );
        Shell_NotifyIcon( NIM_ADD, &data_.iconData );
        Shell_NotifyIcon( NIM_SETVERSION, &data_.iconData );
    }

    WindowsTaskIcon::~WindowsTaskIcon()
    {
        HINSTANCE hInstance = GetModuleHandle( NULL );
        Shell_NotifyIcon( NIM_DELETE, &data_.iconData );
        UnregisterClass( CLASS_NAME, hInstance );
    }

    void WindowsTaskIcon::SetName( const WString& newName )
    {
        name_ = newName;
        wcscpy_s( data_.iconData.szTip, name_.c_str() );
        Shell_NotifyIcon( NIM_MODIFY, &data_.iconData );
    }

    const WString& WindowsTaskIcon::GetName() const
    {
        return name_;
    }

    void WindowsTaskIcon::AddMenuItem( const TaskIconMenuItemCreateInfo& menu )
    {
        if ( data_.popupMenu == NULL )
        {
            data_.popupMenu = CreatePopupMenu();
        }

        WindowsMenuItem& item = data_.menuItems.emplace_back
        (
            WindowsMenuItem
            {
                .label = menu.label,
                .userData = menu.userData,
                .callback = menu.callback
            }
        );
        item.itemInfo.cbSize = sizeof( MENUITEMINFO );
        item.itemInfo.fMask = MIIM_STRING | MIIM_ID;
        item.itemInfo.fType = MFT_STRING;
        item.itemInfo.wID = 0x99 - (UINT)data_.menuItems.size();
        item.itemInfo.dwTypeData = item.label.data();
        item.itemInfo.cch = 5;

        InsertMenuItem( data_.popupMenu, 0, FALSE, &item.itemInfo );
    }

    TaskIcon* PlatformCreateTaskIcon( const TaskIconCreateInfo& info )
    {
        return new WindowsTaskIcon( info );
    }
}