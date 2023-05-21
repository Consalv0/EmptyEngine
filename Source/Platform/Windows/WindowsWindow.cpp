
#include "CoreMinimal.h"
#include "Core/Application.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"
#include "Graphics/PixelMap.h"

#include "Utils/TextFormatting.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>


namespace EEngine
{
	int OnSDLWindowInputEvent( void* userData, SDL_Event* eventPtr )
	{
		return WindowInputEventHandler( userData, eventPtr );
	}

	void WindowsWindow::Initialize()
	{
		if ( device_ != NULL || windowHandle_ != NULL ) return;

		if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC ) != 0 )
		{
			LOG_CORE_CRITICAL( L"Failed to initialize SDL3: {0}\n", Text::NarrowToWide( SDL_GetError() ) );
			return;
		}

		SDL_AddEventWatch( OnSDLWindowInputEvent, (void*)this );

		if ( (windowHandle_ = SDL_CreateWindowWithPosition(
			Text::WideToNarrow( name ).c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width, height,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_KEYBOARD_GRABBED | mode_ // | GraphicsDevice::GetWindowMode()
		)) == NULL )
		{
			LOG_CORE_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name, Text::NarrowToWide( SDL_GetError() ) );
			return;
		}

		SDL_SetWindowData( (SDL_Window*)windowHandle_, "WindowData", this );

		device_ = std::unique_ptr<CPUGraphicsDevice>( new CPUGraphicsDevice() );

		WindowsInput::GetInputInstance()->CheckForConnectedJoysticks();

		SwapChainDescription swapChainDesc {};
		swapChainDesc.width = width;
		swapChainDesc.height = height;
		swapChainDesc.fullscreen = mode_;
		swapChainDesc.vsync = vsync;
		device_->CreateSwapChain( swapChainDesc, windowHandle_, swapChain_ );
	}

	WindowsWindow::WindowsWindow( const WindowProperties& Parameters )
	{
		device_ = NULL;
		windowHandle_ = NULL;
		width = Parameters.width;
		height = Parameters.height;
		name = Parameters.name;
		mode_ = Parameters.windowMode;
		Initialize();
	}

	WindowsWindow::~WindowsWindow()
	{
		Terminate();
		SDL_Quit();
	}

	void WindowsWindow::Resize( const uint32_t& Wth, const uint32_t& Hht )
	{
		if ( width != Wth || height != Hht )
		{
			width = Wth; height = Hht;
			SDL_SetWindowSize( (SDL_Window*)(windowHandle_), Wth, Hht );

			WindowResizeEvent Event( width, height );
			WindowEventCallback( Event );
		}
	}

	void WindowsWindow::SetName( const WString& NewName )
	{
		name = NewName;
		SDL_SetWindowTitle( (SDL_Window*)(windowHandle_), Text::WideToNarrow( name ).c_str() );
	}

	Vector2 WindowsWindow::GetMousePosition( bool Clamp )
	{
		float MouseX, MouseY;
		if ( Clamp )
		{
			SDL_GetMouseState( &MouseX, &MouseY );
		}
		else
		{
			int WindowX, WindowY;
			SDL_GetGlobalMouseState( &MouseX, &MouseY );
			SDL_GetWindowPosition( (SDL_Window*)(windowHandle_), &WindowX, &WindowY );
			MouseX -= WindowX;
			MouseY -= WindowY;
		}
		return Vector2( float( MouseX ), float( MouseY ) );
	}

	void WindowsWindow::SetWindowMode( EWindowMode Mode )
	{
		this->mode_ = Mode;
		SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle_), (SDL_bool)Mode );
	}

	EWindowMode WindowsWindow::GetWindowMode() const
	{
		return mode_;
	}

	void WindowsWindow::SetIcon( PixelMap* Icon )
	{
		if ( Icon->GetColorFormat() != PixelFormat_RGBA8 ) return;
		SDL_Surface* Surface = SDL_CreateSurfaceFrom(
			(void*)Icon->PointerToValue(),
			Icon->GetWidth(), Icon->GetHeight(),
			NULL, SDL_PIXELFORMAT_RGBA32
		);
		SDL_SetWindowIcon( static_cast<SDL_Window*>(windowHandle_), Surface );
		SDL_DestroySurface( Surface );
	}

	void WindowsWindow::BeginFrame()
	{
		CheckInputState();
		device_->RenderPassBegin( swapChain_, 0 );
	}

	void WindowsWindow::EndFrame()
	{
		device_->RenderPassEnd( swapChain_, 0 );
	}

	uint64_t WindowsWindow::GetFrameCount() const
	{
		return device_->GetFrameCount();
	}

	void WindowsWindow::Terminate()
	{
		if ( IsRunning() )
		{
#ifdef EE_DEBUG
			LOG_CORE_DEBUG( L"Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG
			SDL_DestroyWindow( (SDL_Window*)(windowHandle_) );
			windowHandle_ = NULL;
			SDL_DelEventWatch( OnSDLWindowInputEvent, (void*)this );
			device_.reset( NULL );
			// SDL_DelEventWatch( OnResizeWindow, this);
		}
	}

	void WindowsWindow::CheckInputState()
	{
		SDL_Event sdlEvent;
		while ( SDL_PollEvent( &sdlEvent ) ) { }

		auto InputInstance = WindowsInput::GetInputInstance();
		for ( auto& KeyStateIt : InputInstance->keyboardInputState )
		{
			if ( KeyStateIt.second.State & ButtonState_Pressed )
			{
				KeyStateIt.second.FramePressed = Application::GetInstance()->GetWindow().GetFrameCount();
			}
			KeyStateIt.second.State &= ~(ButtonState_Pressed | ButtonState_Released | ButtonState_Typed);
		}
		for ( auto& MouseButtonIt : InputInstance->mouseInputState )
		{
			if ( MouseButtonIt.second.State & ButtonState_Pressed )
			{
				MouseButtonIt.second.FramePressed = Application::GetInstance()->GetWindow().GetFrameCount();
			}
			MouseButtonIt.second.State &= ~(ButtonState_Pressed | ButtonState_Released);
		}
		for ( auto& JoystickIt : InputInstance->joystickButtonState )
		{
			for ( auto& JoystickButtonIt : JoystickIt.second )
			{
				if ( JoystickButtonIt.second.State & ButtonState_Pressed )
				{
					JoystickButtonIt.second.FramePressed = Application::GetInstance()->GetWindow().GetFrameCount();
				}
				JoystickButtonIt.second.State &= ~(ButtonState_Pressed | ButtonState_Released);
			}
		}
	}

	bool WindowsWindow::IsRunning()
	{
		return windowHandle_ != NULL && device_;
	}

	WString WindowsWindow::GetName() const
	{
		return name;
	}

	float WindowsWindow::GetAspectRatio() const
	{
		return (float)width / (float)height;
	}

	int WindowsWindow::GetWidth() const
	{
		return width;
	}

	int WindowsWindow::GetHeight() const
	{
		return height;
	}

	IntVector2 WindowsWindow::GetSize() const
	{
		return IntVector2( width, height );
	}

	IntBox2D WindowsWindow::GetViewport() const
	{
		return IntBox2D( 0, 0, width, height );
	}

	void* WindowsWindow::GetHandle() const
	{
		return windowHandle_;
	}

	Window* Window::Create( const WindowProperties& Parameters )
	{
		return new WindowsWindow( Parameters );
	}
}
