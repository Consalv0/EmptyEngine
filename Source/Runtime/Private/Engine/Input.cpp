
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Engine/Input.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>

namespace EE
{
    JoystickState GJoystickDeviceStates[EE_MAX_GAMEPAD_COUNT];

    GamepadButtonState GGamepadButtonStates[ EE_MAX_GAMEPAD_COUNT ][ EGamepadButton::Gamepad_Button_MAX ];

    ScancodeState GScancodeStates[ EScancode::Scancode_NUM ];

    MouseButtonState GMouseButtonStates[ EMouseButton::Mouse_Button_NUM ];

    Point2 GMousePosition, GRelativeMousePosition;

    int InputEventsHandler_Internal( void* userData, SDL_Event* sdlEvent );

    bool Input::Initialize()
    {
        SDL_AddEventWatch( InputEventsHandler_Internal, (void*)this );
        CheckForConnectedJoysticks();
        return true;
    }

    bool Input::IsKeyState( EScancode keyCode, EButtonState state )
    {
        if ( state & ButtonState_Down ) return GScancodeStates[ keyCode ].framePressed;
        return GScancodeStates[ keyCode ].state & state;
    }

    bool Input::IsMouseState( EMouseButton button, EButtonState state )
    {
        if ( state & ButtonState_Down ) return GMouseButtonStates[ button ].framePressed;
        return GMouseButtonStates[ button ].state & state;
    }

    bool Input::IsButtonState( int index, EGamepadButton button, EButtonState state )
    {
        if ( index == -1 )
        {
            TArray<int> Indices;
            Indices = GetJoysticksConnected();
            if ( Indices.empty() ) index = 0;
            else index = Indices[ 0 ];
        }
        if ( state & ButtonState_Down )
        {
            return GGamepadButtonStates[ index ][ button ].framePressed;
        }
        return GGamepadButtonStates[ index ][ button ].state & state;
    }

    float Input::GetJoystickAxis( int index, EGamepadAxis axis )
    {
        if ( index == -1 )
        {
            TArray<int> indices;
            indices = GetJoysticksConnected();
            if ( indices.empty() ) index = 0;
            else index = indices[ 0 ];
        }
        const JoystickState* joystick = NULL;
        if ( GetJoystickState( index, &joystick ) )
        {
            SDL_Gamepad* gamepad = SDL_GetGamepadFromInstanceID( joystick->instanceID );
            return SDL_GetGamepadAxis( gamepad, (SDL_GamepadAxis)axis ) / 32768.F;
        }
        return 0.F;
    }

    bool Input::GetJoystickState( int index, const JoystickState** state )
    {
        *state = &GJoystickDeviceStates[ index ];
        return (*state)->instanceID;
    }

    void Input::SendHapticImpulse( int index, int channel, float amplitude, int duration )
    {
        if ( index == -1 )
        {
            TArray<int> indices;
            indices = GetJoysticksConnected();
            if ( indices.empty() ) index = 0;
            else index = indices[ 0 ];
        }
        const JoystickState* joystick;
        if ( GetJoystickState( index, &joystick ) && joystick->hapticDevice != NULL )
            SDL_HapticRumblePlay( (SDL_Haptic*)joystick->hapticDevice, amplitude, duration );
    }

    TArray<int> Input::GetJoysticksConnected()
    {
        TArray<int> indices; int count = 0;
        for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
        {
            if ( GetJoystickState( i, NULL ) )
                indices.push_back( count );
            count++;
        }

        return indices;
    }

    void Input::UpdateMouseState()
    {
        SDL_GetMouseState( &GMousePosition.x, &GMousePosition.y );
        int windowX, windowY;
        SDL_GetWindowPosition( (SDL_Window*)GEngine->GetWindow()->GetHandle(), &windowX, &windowY );
        GRelativeMousePosition.x = GMousePosition.x - windowX;
        GRelativeMousePosition.y = GMousePosition.y - windowY;
    }

    const Point2& Input::GetMousePosition( bool realtive ) const
    {
        if ( realtive )
            return GMousePosition;
        else
            return GRelativeMousePosition;
    }

    void Input::Update()
    {
        UpdateMouseState();

        for ( int i = 0; i < Scancode_NUM; i++ )
        {
            if ( GScancodeStates[ i ].state & ButtonState_Pressed )
            {
                GScancodeStates[ i ].framePressed = GEngine->GetFrameCount();
            }
            GScancodeStates[ i ].state &= ~(ButtonState_Pressed | ButtonState_Released | ButtonState_Typed);
        }
        for ( int i = 0; i < Mouse_Button_NUM; i++ )
        {
            if ( GMouseButtonStates[ i ].state & ButtonState_Pressed )
            {
                GMouseButtonStates[ i ].framePressed = GEngine->GetFrameCount();
            }
            GMouseButtonStates[ i ].state &= ~(ButtonState_Pressed | ButtonState_Released);
        }
        for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
        {
            auto gamepad = GGamepadButtonStates[ i ];
            for ( int j = 0; j < Gamepad_Button_MAX; j++ )
            {
                if ( gamepad[ j ].state & ButtonState_Pressed )
                {
                    gamepad[ j ].framePressed = GEngine->GetFrameCount();
                }
                gamepad[ j ].state &= ~(ButtonState_Pressed | ButtonState_Released);
            }
        }
    }

    void Input::InputEventsHandler( const InputEvent& inputEvent )
    {
    }

    int InputEventsHandler_Internal( void* userData, SDL_Event* sdlEvent )
    {
        Input& input = *(Input*)userData; 
        
        auto& keyState = GScancodeStates[ (EScancode)sdlEvent->key.keysym.scancode ];
        auto& mouseState = GMouseButtonStates[ (EMouseButton)sdlEvent->button.button ];
        static int32_t mouseButtonPressedCount[ 255 ] = {
            (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1,
            (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1, (int32_t)-1
        };

        switch ( sdlEvent->type )
        {
        case SDL_EVENT_JOYSTICK_ADDED:
        case SDL_EVENT_GAMEPAD_ADDED:
        {
            bool isGameController = SDL_IsGamepad( sdlEvent->jdevice.which );
            if ( isGameController )
            {
                SDL_Gamepad* gamepad = SDL_OpenGamepad( sdlEvent->jdevice.which );
                SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );

                EName deviceName = EName(
                    Text::NarrowToWide( SDL_GetJoystickName( SDL_GetGamepadJoystick( gamepad ) ) ),
                    sdlEvent->jdevice.which
                );

                JoystickState* addedJoystickState = NULL;
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name == deviceName )
                    {
                        if ( GJoystickDeviceStates[ i ].isConnected == false )
                        {
                            addedJoystickState = &GJoystickDeviceStates[ i ];
                            break;
                        }
                    }
                }
                if ( addedJoystickState == NULL )
                {
                    for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                    {
                        if ( GJoystickDeviceStates[ i ].isConnected == false )
                        {
                            addedJoystickState = &GJoystickDeviceStates[ i ];
                            break;
                        }
                    }
                }
                if ( addedJoystickState == NULL )
                {
                    for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                    {
                        if ( GJoystickDeviceStates[ i ].instanceID == -1 )
                        {
                            GJoystickDeviceStates[ i ] = JoystickState();
                            addedJoystickState = &GJoystickDeviceStates[ i ];
                            break;
                        }
                    }
                }

                SDL_Haptic* ControllerHaptic = SDL_HapticOpenFromJoystick( joystick );
                if ( ControllerHaptic == NULL )
                {
                    addedJoystickState->hasHaptics = false;
                    addedJoystickState->hapticDevice = NULL;
                }
                else
                {
                    if ( SDL_HapticRumbleInit( ControllerHaptic ) == 0 )
                    {
                        addedJoystickState->hasHaptics = true;
                        addedJoystickState->hapticDevice = ControllerHaptic;
                    }
                    else
                    {
                        addedJoystickState->hasHaptics = false;
                        addedJoystickState->hapticDevice = NULL;
                        SDL_HapticClose( ControllerHaptic );
                    }
                }
                addedJoystickState->instanceID = sdlEvent->jdevice.which;
                addedJoystickState->name = deviceName;
                addedJoystickState->isConnected = true;

                EE_LOG_CORE_INFO( L"Device {} Opened", addedJoystickState->name.GetInstanceName() );

            }
            break;
        }

        case SDL_EVENT_JOYSTICK_REMOVED:
        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            SDL_Gamepad* gamepad = SDL_OpenGamepad( sdlEvent->jdevice.which );
            SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );

            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                EName deviceName = EName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name.GetID() == deviceName.GetID() )
                    {
                        joystickState = &GJoystickDeviceStates[ i ];
                        break;
                    }
                }

                if ( joystickState == NULL ) break;
                joystickState->isConnected = false;

                EE_LOG_CORE_INFO( L"Device {} Closed", joystickState->name.GetInstanceName() );

                JoystickConnectionEvent inEvent(
                    joystickState->instanceID, 0
                );
                input.InputEventsHandler( inEvent );

                SDL_HapticClose( (SDL_Haptic*)joystickState->hapticDevice );
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
                JoystickState* joystickState = NULL;
                EName deviceName = EName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
                int index = 0;
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name.GetID() == deviceName.GetID() )
                    {
                        joystickState = &GJoystickDeviceStates[ i ];
                        break;
                    }
                    index++;
                }
                if ( joystickState == NULL ) break;

                auto& joyButtonState = GGamepadButtonStates[ index ][ (EGamepadButton)sdlEvent->jbutton.button ];
                joyButtonState.state = ButtonState_Released;
                joyButtonState.framePressed = 0;

                JoystickButtonReleaseEvent inEvent(
                    joystickState->instanceID, (EGamepadButton)sdlEvent->gbutton.button
                );
                input.InputEventsHandler( inEvent );
            }
            break;
        }

        case SDL_EVENT_JOYSTICK_HAT_MOTION:
        {
            EE_LOG_CORE_WARN( "{}", SDL_NumHaptics() );
            EE_LOG_CORE_INFO( "Hat motion {}, {}", sdlEvent->jhat.hat, sdlEvent->jhat.value );
            break;
        }

        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        {
            SDL_Joystick* joystick = SDL_GetJoystickFromInstanceID( sdlEvent->jbutton.which );
            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                EName deviceName = EName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
                int index = 0;
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name.GetID() == deviceName.GetID() )
                    {
                        joystickState = &GJoystickDeviceStates[ i ];
                        break;
                    }
                    index++;
                }
                if ( joystickState == NULL ) break;

                auto& JoyButtonState = GGamepadButtonStates[ index ][ (EGamepadButton)sdlEvent->gbutton.button ];
                JoyButtonState.state = ButtonState_Pressed;

                EE_LOG_CORE_INFO( "{} Button pressed {}", sdlEvent->gbutton.which, sdlEvent->gbutton.button );

                JoystickButtonPressedEvent inEvent
                (
                    joystickState->instanceID, (EGamepadButton)sdlEvent->gbutton.button
                );
                input.InputEventsHandler( inEvent );
            }
            break;
        }

        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        {
            SDL_Joystick* joystick = SDL_GetJoystickFromInstanceID( sdlEvent->jdevice.which );

            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                EName deviceName = EName( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name.GetID() == deviceName.GetID() )
                    {
                        joystickState = &GJoystickDeviceStates[ i ];
                        break;
                    }
                }
                if ( joystickState == NULL ) break;

                JoystickAxisEvent inEvent(
                    joystickState->instanceID, (EGamepadAxis)sdlEvent->gaxis.axis, sdlEvent->gaxis.value / 32768.F
                );
                input.InputEventsHandler( inEvent );
            }
            break;
        }

        case SDL_EVENT_KEY_DOWN:
        {
            if ( sdlEvent->key.repeat == 0 )
                keyState.state = ButtonState_Pressed | ButtonState_Typed;
            else
                keyState.state = ButtonState_Typed;
            keyState.repetitions = sdlEvent->key.repeat;

            KeyPressedEvent inEvent(
                sdlEvent->key.keysym.scancode,
                (SDL_GetModState() & SDL_KMOD_SHIFT) != 0,
                (SDL_GetModState() & SDL_KMOD_CTRL) != 0,
                (SDL_GetModState() & SDL_KMOD_ALT) != 0,
                (SDL_GetModState() & SDL_KMOD_GUI) != 0,
                sdlEvent->key.repeat
            );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            keyState.state = ButtonState_Released;
            keyState.framePressed = 0;
            keyState.repetitions = 0;

            KeyReleasedEvent inEvent(
                sdlEvent->key.keysym.scancode,
                (SDL_GetModState() & SDL_KMOD_SHIFT) != 0,
                (SDL_GetModState() & SDL_KMOD_CTRL) != 0,
                (SDL_GetModState() & SDL_KMOD_ALT) != 0,
                (SDL_GetModState() & SDL_KMOD_GUI) != 0
            );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_TEXT_INPUT:
        {
            KeyTypedEvent inEvent( sdlEvent->text.text );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            mouseState.state = ButtonState_Pressed;
            mouseState.Clicks = sdlEvent->button.clicks;

            mouseButtonPressedCount[ sdlEvent->button.button ]++;
            MouseButtonPressedEvent inEvent( sdlEvent->button.button, sdlEvent->button.clicks == 2, mouseButtonPressedCount[ sdlEvent->button.button ] );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            mouseState.state = ButtonState_Released;
            mouseState.framePressed = 0;
            mouseState.Clicks = 0;

            mouseButtonPressedCount[ sdlEvent->button.button ] = -1;
            MouseButtonReleasedEvent inEvent( sdlEvent->button.button );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            MouseMovedEvent inEvent( (float)sdlEvent->motion.x, (float)sdlEvent->motion.y, (float)sdlEvent->motion.xrel, (float)sdlEvent->motion.yrel );
            input.InputEventsHandler( inEvent );
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            MouseScrolledEvent inEvent
            (
                (float)sdlEvent->wheel.x, (float)sdlEvent->wheel.y,
                sdlEvent->wheel.direction == SDL_MOUSEWHEEL_FLIPPED
            );
            input.InputEventsHandler( inEvent );
            break;
        }
        }

        return 0;
    }

    void Input::CheckForConnectedJoysticks()
    {
        int count;
        SDL_JoystickID* ids = SDL_GetJoysticks( &count );

        for ( int i = 0; i < count; i++ )
        {
            bool isGamePad = SDL_IsGamepad( i );
            if ( isGamePad == false )
                continue;

            SDL_Gamepad* gamepad = SDL_OpenGamepad( i );
            SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );

            EName deviceName = EName( Text::NarrowToWide( SDL_GetJoystickName( SDL_GetGamepadJoystick( gamepad ) ) ), ids[ i ] );
            JoystickState* connectedJoystick = NULL;
            for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
            {
                if ( GJoystickDeviceStates[ i ].name == deviceName )
                {
                    if ( GJoystickDeviceStates[ i ].isConnected )
                        return;
                    connectedJoystick = &GJoystickDeviceStates[ i ];
                    break;
                }
            }
            if ( connectedJoystick == NULL )
            {
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].isConnected == false )
                    {
                        connectedJoystick = &GJoystickDeviceStates[ i ];
                        break;
                    }
                }
            }
            if ( connectedJoystick == NULL )
            {
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].instanceID == -1 )
                    {
                        GJoystickDeviceStates[ i ] = JoystickState();
                        connectedJoystick = &GJoystickDeviceStates[ i ];
                        break;
                    }
                }
            }

            SDL_Haptic* haptic = SDL_HapticOpenFromJoystick( joystick );
            if ( haptic == NULL )
            {
                connectedJoystick->hasHaptics = false;
                connectedJoystick->hapticDevice = NULL;
            }
            else
            {
                if ( SDL_HapticRumbleInit( haptic ) == 0 )
                {
                    connectedJoystick->hasHaptics = true;
                    connectedJoystick->hapticDevice = haptic;
                }
                else
                {
                    connectedJoystick->hasHaptics = false;
                    connectedJoystick->hapticDevice = NULL;
                    SDL_HapticClose( haptic );
                }
            }
            connectedJoystick->instanceID = ids[ i ];
            connectedJoystick->name = deviceName;
            connectedJoystick->isConnected = true;
        }
    }
}