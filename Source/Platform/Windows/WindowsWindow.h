
#include "Engine/Window.h"

namespace EE
{
    class WindowsWindow : public Window
    {
    public:
        typedef Window Super;

    protected:

    public:
        WindowsWindow( const WindowCreateDescription& parameters = WindowCreateDescription() );

        ~WindowsWindow();

        bool MakeTransparent( bool enable, const uint8& a ) override;
    };
}