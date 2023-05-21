
#include "Core/Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Platform/DeviceFunctions.h"
#include "Events/Property.h"
#include "Math/MathUtils.h"
#include "Math/IntVector2.h"

namespace EEngine
{
	class WindowsWindow : public Window
	{
	public:
		typedef Window Super;

		struct
		{
			WString name;
			int width;
			int height;
			bool vsync;
			WindowEventCallbackFunction WindowEventCallback;
			InputEventCallbackFunction InputEventCallback;
		};

		void Initialize();

	public:
		WindowsWindow( const WindowProperties& parameters = WindowProperties() );

		virtual ~WindowsWindow();

		//* Resize the size of the window
		void Resize( const uint32_t& Width, const uint32_t& Height );

		//* Rename the window title
		void SetName( const WString& NewName );

		//* Get mouse position in screen coordinates relative to the upper left position of this window
		struct Vector2 GetMousePosition( bool Clamp = false );

		//* Set the window display mode
		void SetWindowMode( EWindowMode Mode ) override;

		//* Get the window display mode
		EWindowMode GetWindowMode() const override;

		//* Sets the window icon
		void SetIcon( class PixelMap* Icon );

		//* Window update events
		virtual void BeginFrame() override;

		//* Window frame events
		virtual void EndFrame() override;

		//* Get the total frames renderized
		uint64_t GetFrameCount() const override;

		//* Terminates this window
		void Terminate();

		void CheckInputState();

		//* Returns true if window has been created
		virtual bool IsRunning() override;

		//* Get the window title name
		virtual WString GetName() const override;

		//* Get the aspect of width divided by height in pixels of the window
		virtual float GetAspectRatio() const override;

		//* Get the width in pixels of the window
		virtual int GetWidth() const override;

		//* Get the height in pixels of the window
		virtual int GetHeight() const override;

		//* Get the size of the window in pixels
		virtual IntVector2 GetSize() const override;

		//* Get the viewport of the window in pixels
		virtual IntBox2D GetViewport() const override;

		//* Get the platform Window pointer
		virtual void* GetHandle() const override;

		//* Set callback communication with window events
		inline void SetWindowEventCallback( const WindowEventCallbackFunction& Callback ) override { WindowEventCallback = Callback; }

		//* Set callback communication with input events
		inline void SetInputEventCallback( const InputEventCallbackFunction& Callback ) override { InputEventCallback = Callback; }

	};
}