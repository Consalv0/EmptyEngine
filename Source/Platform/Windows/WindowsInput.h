#pragma once

#include "Core/Input.h"

namespace EEngine
{
	class WindowsInput : public Input
	{
	public:
		static WindowsInput* GetInputInstance();

		TArray<DeviceJoystickState> joystickDeviceState;

		TDictionary<int, TDictionary<EJoystickButton, InputJoystickState>> joystickButtonState;

		TDictionary<EScancode, InputScancodeState> keyboardInputState;

		TDictionary<EMouseButton, InputMouseButtonState> mouseInputState;

	protected:
		friend class WindowsWindow;

		virtual bool IsKeyStateNative( EScancode keyCode, int state ) override;

		virtual bool IsMouseStateNative( EMouseButton button, int state ) override;

		virtual bool IsButtonStateNative( int index, EJoystickButton keyCode, int state ) override;

		virtual float GetAxisNative( int index, EJoystickAxis axis ) override;

		virtual DeviceJoystickState& GetJoystickStateNative( int index ) override;

		virtual void SendHapticImpulseNative( int index, int channel, float amplitude, int duration ) override;

		virtual TArray<int> GetJoysticksConnected() override;

		virtual void CheckForConnectedJoysticks() override;

		virtual Vector2 GetMousePositionNative( bool clamp ) override;

		virtual float GetMouseXNative( bool clamp ) override;

		virtual float GetMouseYNative( bool clamp ) override;

	};

	int WindowInputEventHandler( void* userData, void* sdlEvent );
}

