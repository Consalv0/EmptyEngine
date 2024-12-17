
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Engine/Ticker.h"
#include "Engine/Input.h"

#include <Utils/VariableWatcher.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>

namespace EE
{
    JoystickState GJoystickDeviceStates[ EE_MAX_GAMEPAD_COUNT ];

    GamepadButtonState GGamepadButtonStates[ EE_MAX_GAMEPAD_COUNT ][ EGamepadButton::GamepadButton_NUM ];

    KeyboardKeyState GKeyboardKeyStates[ EKeyboardKey::KeyboardKey_NUM ];

    MouseButtonState GMouseButtonStates[ EMouseButton::MouseButton_NUM ];

    float GMousePositionX, GMousePositionY, GRelativeMousePositionX, GRelativeMousePositionY;

    bool InputEventsHandler_Internal( void* userData, SDL_Event* sdlEvent );

    bool Input::Initialize()
    {
        SDL_AddEventWatch( InputEventsHandler_Internal, (void*)this );
        CheckForConnectedJoysticks();
        return true;
    }

    bool Input::IsKeyState( EKeyboardKey keyCode, EButtonStateFlags state, uint64 frame )
    {
        if ( frame != UINT64_MAX && frame != GKeyboardKeyStates[ keyCode ].frameDown )
            return false;
        return GKeyboardKeyStates[ keyCode ].state & state;
    }

    bool Input::IsMouseState( EMouseButton button, EButtonStateFlags state, uint64 frame )
    {
        if ( frame != UINT64_MAX && frame != GMouseButtonStates[ button ].frameDown )
            return false;
        return GMouseButtonStates[ button ].state & state;
    }

    bool Input::IsButtonState( int index, EGamepadButton button, EButtonStateFlags state, uint64 frame )
    {
        if ( index == -1 )
        {
            TArray<int> Indices;
            Indices = GetJoysticksConnected();
            if ( Indices.empty() ) index = 0;
            else index = Indices[ 0 ];
        }

        if ( frame != UINT64_MAX && frame != GGamepadButtonStates[ index ][ button ].frameDown )
            return false;
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
            SDL_Gamepad* gamepad = SDL_GetGamepadFromID( joystick->instanceID );
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
            SDL_PlayHapticRumble( (SDL_Haptic*)joystick->hapticDevice, amplitude, duration );
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
        SDL_GetMouseState( &GMousePositionX, &GMousePositionY );
        if ( GEngine->GetWindowCount() > 0 )
        {
            int windowX, windowY;
            SDL_GetWindowPosition( (SDL_Window*)GEngine->GetWindow( 0 )->GetWindowHandle(), &windowX, &windowY );
            GRelativeMousePositionX = GMousePositionX - windowX;
            GRelativeMousePositionY = GMousePositionY - windowY;
        }
        else
        {
            GRelativeMousePositionX = GMousePositionX;
            GRelativeMousePositionY = GMousePositionY;
        }
    }

    void Input::GetMousePosition( float* x, float* y, bool realtive ) const
    {
        if ( realtive )
        {
            *x = GMousePositionX; *y = GMousePositionY;
        }
        else
        {
            *x = GRelativeMousePositionX; *y = GRelativeMousePositionY;
        }
    }

    void Input::Update()
    {
        UpdateMouseState(); 
        
        for ( int i = 0; i < KeyboardKey_NUM; i++ )
        {
            if ( GKeyboardKeyStates[ i ].state & ButtonState_Typed )
            {
                GKeyboardKeyStates[ i ].state &= ~ButtonState_Typed;
            }
        }
    }

    bool InputEventsHandler_Internal( void* userData, SDL_Event* sdlEvent )
    {
        Input& input = *(Input*)userData; 

        auto& keyState = GKeyboardKeyStates[ (EKeyboardKey)sdlEvent->key.scancode ];
        auto& mouseState = GMouseButtonStates[ (EMouseButton)sdlEvent->button.button ];
        static int32 mouseButtonPressedCount[ 255 ] {};

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

                Name deviceName = Name(
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

                SDL_Haptic* ControllerHaptic = SDL_OpenHapticFromJoystick( joystick );
                if ( ControllerHaptic == NULL )
                {
                    addedJoystickState->hasHaptics = false;
                    addedJoystickState->hapticDevice = NULL;
                }
                else
                {
                    if ( SDL_InitHapticRumble( ControllerHaptic ) == 0 )
                    {
                        addedJoystickState->hasHaptics = true;
                        addedJoystickState->hapticDevice = ControllerHaptic;
                    }
                    else
                    {
                        addedJoystickState->hasHaptics = false;
                        addedJoystickState->hapticDevice = NULL;
                        SDL_CloseHaptic( ControllerHaptic );
                    }
                }
                addedJoystickState->instanceID = sdlEvent->jdevice.which;
                addedJoystickState->name = deviceName;
                addedJoystickState->isConnected = true;

                EE_LOG_INFO( L"Device {} Opened", addedJoystickState->name.GetInstanceName() );

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
                Name deviceName = Name( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
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

                EE_LOG_INFO( L"Device {} Closed", joystickState->name.GetInstanceName() );

                SDL_CloseHaptic( (SDL_Haptic*)joystickState->hapticDevice );
                SDL_CloseJoystick( joystick );
            }
            break;
        }

        case SDL_EVENT_JOYSTICK_BUTTON_UP:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
        {
            SDL_Joystick* joystick = SDL_GetJoystickFromID( sdlEvent->jbutton.which );
            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                Name deviceName = Name( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
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
                joyButtonState.state = ButtonState_Up;
                joyButtonState.frameDown = GEngine->GetFrameCount();
            }
            break;
        }

        case SDL_EVENT_JOYSTICK_HAT_MOTION:
        {
            EE_LOG_INFO( "Hat motion {}, {}", sdlEvent->jhat.hat, sdlEvent->jhat.value );
            break;
        }

        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        {
            SDL_Joystick* joystick = SDL_GetJoystickFromID( sdlEvent->jbutton.which );
            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                Name deviceName = Name( Text::NarrowToWide( SDL_GetJoystickName( joystick ) ), sdlEvent->jdevice.which );
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

                auto& joyButtonState = GGamepadButtonStates[ index ][ (EGamepadButton)sdlEvent->gbutton.button ];
                joyButtonState.state = ButtonState_Down;
                joyButtonState.frameDown = GEngine->GetFrameCount();

                EE_WATCH( sdlEvent->gbutton.button );
                EE_LOG_INFO( "{} Button pressed {}", sdlEvent->gbutton.which, sdlEvent->gbutton.button );
            }
            break;
        }

        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        {
            SDL_Joystick* joystick = SDL_GetJoystickFromID( sdlEvent->jdevice.which );

            if ( SDL_JoystickConnected( joystick ) )
            {
                JoystickState* joystickState = NULL;
                Name deviceName = Name( SDL_GetJoystickName( joystick ), sdlEvent->jdevice.which );
                for ( int i = 0; i < EE_MAX_GAMEPAD_COUNT; i++ )
                {
                    if ( GJoystickDeviceStates[ i ].name.GetID() == deviceName.GetID() )
                    {
                        joystickState = &GJoystickDeviceStates[ i ];
                        break;
                    }
                }
                if ( joystickState == NULL ) break;

                // NChar* s = new NChar[]{ SDL_GetJoystickName( joystick ) };
                // s[std::strlen( s ) - 1] = ('0' + sdlEvent->gaxis.axis);
                EE_WATCH( sdlEvent->gaxis.value / 32768.F );
            }
            break;
        }

        case SDL_EVENT_KEY_DOWN:
        {
            if ( sdlEvent->key.repeat == 0 )
            {
                keyState.state = ButtonState_Down | ButtonState_Typed;
                keyState.frameDown = GEngine->GetFrameCount();
            }
            else
            {
                keyState.state |= ButtonState_Typed;
            }
            keyState.repetitions = sdlEvent->key.repeat;
            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            keyState.state = ButtonState_Up;
            keyState.frameDown = GEngine->GetFrameCount();
            keyState.repetitions = 0;
            break;
        }

        case SDL_EVENT_TEXT_INPUT:
        {
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            mouseState.state = ButtonState_Down;
            mouseState.frameDown = GEngine->GetFrameCount();
            mouseState.clicks = sdlEvent->button.clicks;

            EE_WATCH( mouseState.frameDown );

            mouseButtonPressedCount[ sdlEvent->button.button ]++;
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            mouseState.state = ButtonState_Up;
            mouseState.frameDown = GEngine->GetFrameCount();
            mouseState.clicks = 0;

            mouseButtonPressedCount[ sdlEvent->button.button ] = -1;
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            break;
        }
        }

        return true;
    }

    void Input::CheckForConnectedJoysticks()
    {
        int count;
        SDL_JoystickID* ids = SDL_GetJoysticks( &count );

        for ( int i = 0; i < count; i++ )
        {
            bool isGamePad = SDL_IsGamepad( ids[ i ] );
            if ( isGamePad == false )
                continue;

            SDL_Gamepad* gamepad = SDL_OpenGamepad( ids[ i ] );
            SDL_Joystick* joystick = SDL_GetGamepadJoystick( gamepad );

            Name deviceName = Name( Text::NarrowToWide( SDL_GetJoystickName( SDL_GetGamepadJoystick( gamepad ) ) ), ids[ i ] );
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

            SDL_Haptic* haptic = SDL_OpenHapticFromJoystick( joystick );
            if ( haptic == NULL )
            {
                connectedJoystick->hasHaptics = false;
                connectedJoystick->hapticDevice = NULL;
            }
            else
            {
                if ( SDL_InitHapticRumble( haptic ) == 0 )
                {
                    connectedJoystick->hasHaptics = true;
                    connectedJoystick->hapticDevice = haptic;
                }
                else
                {
                    connectedJoystick->hasHaptics = false;
                    connectedJoystick->hapticDevice = NULL;
                    SDL_CloseHaptic( haptic );
                }
            }
            connectedJoystick->instanceID = ids[ i ];
            connectedJoystick->name = deviceName;
            connectedJoystick->isConnected = true;
        }
    }
}