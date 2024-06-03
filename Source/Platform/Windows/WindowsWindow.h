
#include "Engine/Window.h"

namespace EE
{
    class WindowsWindow : public Window
    {
    public:
        typedef Window Super;

    protected:

    public:
        WindowsWindow( const WindowParameters& parameters = WindowParameters() );

        ~WindowsWindow();

        bool MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a );
    };
}