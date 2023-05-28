
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
		struct
		{
			WString name_;
			int width_;
			int height_;
			bool vsync_;
		};

		void Initialize();

	public:
		WindowsWindow( const WindowProperties& parameters = WindowProperties() );

		~WindowsWindow();

		//* Resize the size of the window
		void Resize( const uint32_t& width, const uint32_t& height );

		//* Rename the window title
		void SetName( const WString& newName );

		//* Set the window display mode
		void SetWindowMode( EWindowMode Mode ) override;

		//* Get the window display mode
		EWindowMode GetWindowMode() const override;

		//* Sets the window icon
		void SetIcon( class PixelMap* Icon );

		//* Window update events
		void BeginFrame() override;

		//* Window frame events
		void EndFrame() override;

		//* Get the total frames renderized
		uint64_t GetFrameCount() const override;

		//* Terminates this window
		void Terminate();

		//* Get the window title name
		WString GetName() const override;

		//* Get the aspect of width divided by height in pixels of the window
		float GetAspectRatio() const override;

		//* Get the width in pixels of the window
		int GetWidth() const override;

		//* Get the height in pixels of the window
		int GetHeight() const override;

		//* Get the size of the window in pixels
		IntVector2 GetSize() const override;

		//* Get the viewport of the window in pixels
		IntBox2D GetViewport() const override;

		//* Get the platform Window pointer
		void* GetHandle() const override;
	};
}