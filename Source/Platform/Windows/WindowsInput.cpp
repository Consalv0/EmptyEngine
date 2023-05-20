
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include <SDL3/SDL_events.h>
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL.h>

namespace EEngine
{
	Input* Input::Instance = new WindowsInput();

	WindowsInput* WindowsInput::GetInputInstance()
	{
		return static_cast<WindowsInput*>(Input::Instance);
	}

	bool WindowsInput::IsKeyStateNative( EScancode KeyCode, int State )
	{
		if ( State == BS_Down ) return KeyboardInputState[ KeyCode ].FramePressed;
		return KeyboardInputState[ KeyCode ].State & State;
	}

	bool WindowsInput::IsMouseStateNative( EMouseButton Button, int State )
	{
		if ( State == BS_Down ) return MouseInputState[ Button ].FramePressed;
		return MouseInputState[ Button ].State & State;
	}

	bool WindowsInput::IsButtonStateNative( int Index, EJoystickButton Button, int State )
	{
		if ( Index == -1 )
		{
			TArray<int> Indices;
			Indices = GetJoysticksConnected();
			if ( Indices.empty() ) Index = 0;
			else Index = Indices[ 0 ];
		}
		if ( State == BS_Down ) return JoystickButtonState[ Index ][ Button ].FramePressed;
		return JoystickButtonState[ Index ][ Button ].State & State;
	}

	float WindowsInput::GetAxisNative( int Index, EJoystickAxis Axis )
	{
		if ( Index == -1 )
		{
			TArray<int> Indices;
			Indices = GetJoysticksConnected();
			if ( Indices.empty() ) Index = 0;
			else Index = Indices[ 0 ];
		}
		DeviceJoystickState& Joystick = GetJoystickStateNative( Index );
		SDL_Gamepad* GController = SDL_GetGamepadFromInstanceID( Joystick.InstanceID );
		return SDL_GetGamepadAxis( GController, (SDL_GamepadAxis)Axis ) / 32768.F;
		return 0.F;
	}

	DeviceJoystickState& WindowsInput::GetJoystickStateNative( int Index )
	{
		if ( Index >= joystickDeviceState.size() ) joystickDeviceState.resize( Index + 1 );
		return joystickDeviceState[ Index ];
	}

	void WindowsInput::SendHapticImpulseNative( int Index, int Channel, float Amplitude, int Duration )
	{
		if ( Index == -1 )
		{
			TArray<int> Indices;
			Indices = GetJoysticksConnected();
			if ( Indices.empty() ) Index = 0;
			else Index = Indices[ 0 ];
		}
		DeviceJoystickState& Joystick = GetJoystickStateNative( Index );
		if ( Joystick.HapticDevice != NULL )
			SDL_HapticRumblePlay( (SDL_Haptic*)Joystick.HapticDevice, Amplitude, Duration );
	}

	TArray<int> WindowsInput::GetJoysticksConnected()
	{
		TArray<int> Indices; int Count = 0;
		for ( auto& DeviceState : joystickDeviceState )
		{
			if ( DeviceState.bConnected ) Indices.push_back( Count );
			Count++;
		}

		return Indices;
	}

	void WindowsInput::CheckForConnectedJoysticks()
	{
		int count;
		SDL_JoystickID* ids = SDL_GetJoysticks( &count );

		for ( int i = 0; i < count; i++ )
		{
			bool isGamePad = SDL_IsGamepad( i );
			if ( isGamePad )
			{
				SDL_Gamepad* gamepad = SDL_OpenGamepad( i );
				auto& devicesState = WindowsInput::GetInputInstance()->joystickDeviceState;
				SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );
				IName deviceName = IName( Text::NarrowToWide( SDL_GetJoystickName( SDL_GetGamepadJoystick( gamepad ) ) ), ids[ i ] );
				DeviceJoystickState* joystickDeviceState = NULL;
				for ( auto& state : devicesState )
				{
					if ( state.name.GetDisplayName() == deviceName.GetDisplayName() )
					{
						if ( state.bConnected ) return;
						joystickDeviceState = &state;
						break;
					}
				}
				if ( joystickDeviceState == NULL )
				{
					for ( auto& state : devicesState )
					{
						if ( state.bConnected == false )
						{
							joystickDeviceState = &state;
							break;
						}
					}
				}
				if ( joystickDeviceState == NULL )
				{
					devicesState.push_back( DeviceJoystickState() );
					joystickDeviceState = &devicesState[ WindowsInput::GetInputInstance()->joystickDeviceState.size() - 1 ];
				}

				SDL_Haptic* ControllerHaptic = SDL_HapticOpenFromJoystick( joystick );
				if ( ControllerHaptic == NULL )
				{
					joystickDeviceState->bHaptics = false;
					joystickDeviceState->HapticDevice = NULL;
				}
				else
				{
					if ( SDL_HapticRumbleInit( ControllerHaptic ) == 0 )
					{
						joystickDeviceState->bHaptics = true;
						joystickDeviceState->HapticDevice = ControllerHaptic;
					}
					else
					{
						joystickDeviceState->bHaptics = false;
						joystickDeviceState->HapticDevice = NULL;
						SDL_HapticClose( ControllerHaptic );
					}
				}
				joystickDeviceState->InstanceID = ids[ i ];
				joystickDeviceState->name = deviceName;
				joystickDeviceState->bConnected = true;
			}
		}
	}

	Vector2 WindowsInput::GetMousePositionNative( bool Clamp )
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
			SDL_GetWindowPosition( (SDL_Window*)Application::GetInstance()->GetWindow().GetHandle(), &WindowX, &WindowY );
			MouseX -= WindowX;
			MouseY -= WindowY;
		}
		return Vector2( MouseX, MouseY );
	}

	float WindowsInput::GetMouseXNative( bool Clamp )
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
			SDL_GetWindowPosition( (SDL_Window*)Application::GetInstance()->GetWindow().GetHandle(), &WindowX, &WindowY );
			MouseX -= WindowX;
		}
		return MouseX;
	}

	float WindowsInput::GetMouseYNative( bool Clamp )
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
			SDL_GetWindowPosition( (SDL_Window*)Application::GetInstance()->GetWindow().GetHandle(), &WindowX, &WindowY );
			MouseY -= WindowY;
		}
		return MouseY;
	}

	int OnSDLWindowInputEvent( void* userData, SDL_Event* sdlEvent )
	{
		auto& KeyState = WindowsInput::GetInputInstance()->KeyboardInputState[ (EScancode)sdlEvent->key.keysym.scancode ];
		auto& MouseState = WindowsInput::GetInputInstance()->MouseInputState[ (EMouseButton)sdlEvent->button.button ];
		WindowsWindow& data = *(WindowsWindow*)userData;
		static int32_t MouseButtonPressedCount[ 255 ] = {
			(int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1,
			(int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1
		};

		if ( sdlEvent->type >= SDL_EVENT_WINDOW_FIRST && sdlEvent->type <= SDL_EVENT_WINDOW_LAST )
		{
			if ( sdlEvent->window.windowID != SDL_GetWindowID( (SDL_Window*)data.GetHandle() ) )
				return 0;
		}

		switch ( sdlEvent->type )
		{
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		case SDL_EVENT_WINDOW_RESIZED:
		{
			data.Resize( sdlEvent->window.data1, sdlEvent->window.data2 );
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			WindowCloseEvent WinEvent;
			data.WindowEventCallback( WinEvent );
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		{
			WindowGainFocusEvent WinEvent;
			data.WindowEventCallback( WinEvent );
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_LOST:
		{
			WindowLostFocusEvent WinEvent;
			data.WindowEventCallback( WinEvent );
			break;
		}

		case SDL_EVENT_JOYSTICK_ADDED:
		case SDL_EVENT_GAMEPAD_ADDED:
		{
			bool isGameController = SDL_IsGamepad( sdlEvent->jdevice.which );
			if ( isGameController )
			{
				SDL_Gamepad* gamepad = SDL_OpenGamepad( sdlEvent->jdevice.which );
				auto& devicesState = WindowsInput::GetInputInstance()->joystickDeviceState;
				SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );
				IName deviceName = IName(
					Text::NarrowToWide( SDL_GetJoystickName( SDL_GetGamepadJoystick( gamepad  ) ) ),
					sdlEvent->jdevice.which
				);
				DeviceJoystickState* joystickDeviceState = NULL;
				for ( auto& state : devicesState )
				{
					if ( state.name.GetDisplayName() == deviceName.GetDisplayName() )
					{
						if ( state.bConnected ) return 0;
						joystickDeviceState = &state;
						break;
					}
				}
				if ( joystickDeviceState == NULL )
				{
					for ( auto& state : devicesState )
					{
						if ( state.bConnected == false )
						{
							joystickDeviceState = &state;
							break;
						}
					}
				}
				if ( joystickDeviceState == NULL )
				{
					devicesState.push_back( DeviceJoystickState() );
					joystickDeviceState = &devicesState[ WindowsInput::GetInputInstance()->joystickDeviceState.size() - 1 ];
				}

				SDL_Haptic* ControllerHaptic = SDL_HapticOpenFromJoystick( joystick );
				if ( ControllerHaptic == NULL )
				{
					joystickDeviceState->bHaptics = false;
					joystickDeviceState->HapticDevice = NULL;
				}
				else
				{
					if ( SDL_HapticRumbleInit( ControllerHaptic ) == 0 )
					{
						joystickDeviceState->bHaptics = true;
						joystickDeviceState->HapticDevice = ControllerHaptic;
					}
					else
					{
						joystickDeviceState->bHaptics = false;
						joystickDeviceState->HapticDevice = NULL;
						SDL_HapticClose( ControllerHaptic );
					}
				}
				joystickDeviceState->InstanceID = sdlEvent->jdevice.which;
				joystickDeviceState->name = deviceName;
				joystickDeviceState->bConnected = true;

				LOG_CORE_INFO( L"Device {} Opened", joystickDeviceState->name.GetInstanceName() );

				JoystickConnectionEvent InEvent(
					joystickDeviceState->InstanceID, 1
				);
				data.InputEventCallback( InEvent );
			}
			break;
		}

		case SDL_EVENT_JOYSTICK_REMOVED:
		case SDL_EVENT_GAMEPAD_REMOVED:
		{
			SDL_Gamepad* gamepad = SDL_OpenGamepad( sdlEvent->jdevice.which );
			auto& devicesState = WindowsInput::GetInputInstance()->joystickDeviceState;
			SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );

			if ( SDL_JoystickConnected( joystick ) )
			{
				DeviceJoystickState* joystickDeviceState = NULL;
				IName deviceName = IName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
				for ( auto& state : WindowsInput::GetInputInstance()->joystickDeviceState )
				{
					if ( state.name.GetID() == deviceName.GetID() )
					{
						joystickDeviceState = &state;
						break;
					}
				}

				if ( joystickDeviceState == NULL ) break;
				joystickDeviceState->bConnected = false;

				LOG_CORE_INFO( L"Device {} Closed", joystickDeviceState->name.GetInstanceName() );

				JoystickConnectionEvent InEvent(
					joystickDeviceState->InstanceID, 0
				);
				data.InputEventCallback( InEvent );

				SDL_HapticClose( (SDL_Haptic*)joystickDeviceState->HapticDevice );
				SDL_CloseJoystick( joystick );
			}
			break;
		}

		case SDL_EVENT_JOYSTICK_BUTTON_UP:
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
		{
			SDL_Joystick* joystick = SDL_GetJoystickFromInstanceID( sdlEvent->jbutton.which );
			if ( SDL_JoystickConnected( joystick ) )
			{
				DeviceJoystickState* joystickDeviceState = NULL;
				IName deviceName = IName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
				int Index = 0;
				for ( auto& state : WindowsInput::GetInputInstance()->joystickDeviceState )
				{
					if ( state.name.GetID() == deviceName.GetID() )
					{
						joystickDeviceState = &state;
						break;
					}
					Index++;
				}
				if ( joystickDeviceState == NULL ) break;

				auto& joyButtonState = WindowsInput::GetInputInstance()->JoystickButtonState[ Index ][ (EJoystickButton)sdlEvent->jbutton.button ];
				joyButtonState.State = BS_Released;
				joyButtonState.FramePressed = 0;

				JoystickButtonReleasedEvent inEvent(
					joystickDeviceState->InstanceID, (EJoystickButton)sdlEvent->gbutton.button
				);
				data.InputEventCallback( inEvent );
			}
			break;
		}

		case SDL_EVENT_JOYSTICK_HAT_MOTION:
		{
			LOG_CORE_WARN( "{}", SDL_NumHaptics() );
			LOG_CORE_INFO( "Hat motion {}, {}", sdlEvent->jhat.hat, sdlEvent->jhat.value );
			break;
		}

		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		{
			SDL_Joystick* joystick = SDL_GetJoystickFromInstanceID( sdlEvent->jbutton.which );
			if ( SDL_JoystickConnected( joystick ) )
			{
				DeviceJoystickState* joystickDeviceState = NULL;
				IName deviceName = IName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
				int index = 0;
				for ( auto& state : WindowsInput::GetInputInstance()->joystickDeviceState )
				{
					if ( state.name.GetID() == deviceName.GetID() )
					{
						joystickDeviceState = &state;
						break;
					}
					index++;
				}
				if ( joystickDeviceState == NULL ) break;

				auto& JoyButtonState = WindowsInput::GetInputInstance()->JoystickButtonState[ index ][ (EJoystickButton)sdlEvent->gbutton.button ];
				JoyButtonState.State = BS_Pressed;

				LOG_CORE_INFO( "{} Button pressed {}", sdlEvent->cbutton.which, sdlEvent->cbutton.button );

				JoystickButtonPressedEvent InEvent(
					joystickDeviceState->InstanceID, (EJoystickButton)sdlEvent->gbutton.button
				);
				data.InputEventCallback( InEvent );
			}
			break;
		}

		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
		{
			SDL_Joystick* joystick = SDL_GetJoystickFromInstanceID( sdlEvent->jdevice.which );

			if ( SDL_JoystickConnected( joystick ) )
			{
				DeviceJoystickState* joystickDeviceState = NULL;
				IName deviceName = IName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
				for ( auto& state : WindowsInput::GetInputInstance()->joystickDeviceState )
				{
					if ( state.name.GetID() == deviceName.GetID() )
					{
						joystickDeviceState = &state;
						break;
					}
				}
				if ( joystickDeviceState == NULL ) break;

				JoystickAxisEvent inEvent(
					joystickDeviceState->InstanceID, (EJoystickAxis)sdlEvent->gaxis.axis, sdlEvent->gaxis.value / 32768.F
				);
				data.InputEventCallback( inEvent );
			}
			break;
		}

		case SDL_EVENT_KEY_DOWN:
		{
			if ( sdlEvent->key.repeat == 0 )
				KeyState.State = BS_Pressed | BS_Typed;
			else
				KeyState.State = BS_Typed;
			KeyState.TypeRepeticions = sdlEvent->key.repeat;

			KeyPressedEvent inEvent(
				sdlEvent->key.keysym.scancode,
				(SDL_GetModState() & SDL_KMOD_SHIFT) != 0,
				(SDL_GetModState() & SDL_KMOD_CTRL) != 0,
				(SDL_GetModState() & SDL_KMOD_ALT) != 0,
				(SDL_GetModState() & SDL_KMOD_GUI) != 0,
				sdlEvent->key.repeat
			);
			data.InputEventCallback( inEvent );
			break;
		}

		case SDL_EVENT_KEY_UP:
		{
			KeyState.State = BS_Released;
			KeyState.FramePressed = 0;
			KeyState.TypeRepeticions = 0;

			KeyReleasedEvent InEvent(
				sdlEvent->key.keysym.scancode,
				(SDL_GetModState() & SDL_KMOD_SHIFT) != 0,
				(SDL_GetModState() & SDL_KMOD_CTRL) != 0,
				(SDL_GetModState() & SDL_KMOD_ALT) != 0,
				(SDL_GetModState() & SDL_KMOD_GUI) != 0
			);
			data.InputEventCallback( InEvent );
			break;
		}

		case SDL_EVENT_TEXT_INPUT:
		{
			KeyTypedEvent inEvent( sdlEvent->text.text );
			data.InputEventCallback( inEvent );
			break;
		}

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			MouseState.State = BS_Pressed;
			MouseState.Clicks = sdlEvent->button.clicks;

			MouseButtonPressedCount[ sdlEvent->button.button ]++;
			MouseButtonPressedEvent inEvent( sdlEvent->button.button, sdlEvent->button.clicks == 2, MouseButtonPressedCount[ sdlEvent->button.button ] );
			data.InputEventCallback( inEvent );
			break;
		}

		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			MouseState.State = BS_Released;
			MouseState.FramePressed = 0;
			MouseState.Clicks = 0;

			MouseButtonPressedCount[ sdlEvent->button.button ] = -1;
			MouseButtonReleasedEvent inEvent( sdlEvent->button.button );
			data.InputEventCallback( inEvent );
			break;
		}

		case SDL_EVENT_MOUSE_MOTION:
		{
			MouseMovedEvent inEvent( (float)sdlEvent->motion.x, (float)sdlEvent->motion.y, (float)sdlEvent->motion.xrel, (float)sdlEvent->motion.yrel );
			data.InputEventCallback( inEvent );
			break;
		}

		case SDL_EVENT_MOUSE_WHEEL:
		{
			MouseScrolledEvent inEvent(
				(float)sdlEvent->wheel.x, (float)sdlEvent->wheel.y,
				sdlEvent->wheel.direction == SDL_MOUSEWHEEL_FLIPPED
			);
			data.InputEventCallback( inEvent );
			break;
		}
		}

		return 0;
	}
}