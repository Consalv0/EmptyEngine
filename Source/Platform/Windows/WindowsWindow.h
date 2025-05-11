
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

        ~WindowsWindow() override;

        bool SetOpacity( const uint8& opacity ) override;

        bool SetPassthrough( bool enable ) override;
    };
}