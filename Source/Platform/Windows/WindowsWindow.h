
#include "Engine/Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Platform/DeviceFunctions.h"
#include "Events/Property.h"
#include "Math/MathUtils.h"
#include "Math/IntVector2.h"

namespace EE
{
    class WindowsWindow : public Window
    {
    public:
        typedef Window Super;

    protected:

    public:
        WindowsWindow( const WindowProperties& parameters = WindowProperties() );

        ~WindowsWindow();

        bool MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a );
    };
}