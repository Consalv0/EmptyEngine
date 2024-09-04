#pragma once

#include "Core/Name.h"
#include "Core/KeyCodes.h"

namespace EE
{
	typedef union {} InputEventData;

	struct JoystickState
	{
		bool isConnected = false;
		int mapping = 0;
		int instanceID = -1;
		bool hasHaptics = false;

        Name name = GEmptyName;
		void* hapticDevice = NULL;
	};

	class Input
	{
	public:
        virtual bool Initialize();

        virtual bool IsKeyState( EScancode keyCode, EButtonStateFlags state, uint64 frame = UINT64_MAX );

		virtual bool IsMouseState( EMouseButton button, EButtonStateFlags state, uint64 frame = UINT64_MAX );

		virtual bool IsButtonState( int index, EGamepadButton keyCode, EButtonStateFlags state, uint64 frame = UINT64_MAX );

		virtual float GetJoystickAxis( int index, EGamepadAxis axis );

		virtual bool GetJoystickState( int index, const JoystickState** state );

		virtual void SendHapticImpulse( int index, int channel, float amplitude, int duration );

		virtual TArray<int> GetJoysticksConnected();

		virtual void CheckForConnectedJoysticks();

		virtual void Update();

        virtual void UpdateMouseState();

		//* Get mouse position in screen coordinates relative to the upper left position of the window
		virtual void GetMousePosition( float* x, float* y, bool realtive ) const;

        virtual void InputEventsHandler( const InputEvent& inputEvent );

	};

	extern Input* GInput;
	
	//* Creates a input manager
	Input* PlatformCreateInput();
}
