#pragma once

#include "Events/Event.h"
#include "Core/KeyCodes.h"
#include "Math/CoreMath.h"

namespace EE
{
    enum class EInputEvent
    {
        InputEvent_KeyPress,
        InputEvent_KeyRelease,
        InputEvent_KeyType,
        InputEvent_MousePressed,
        InputEvent_MouseReleased,
        InputEvent_MouseMoved,
        InputEvent_MouseScrolled,
        InputEvent_JoystickConnection,
        InputEvent_JoystickAxis,
        InputEvent_JoystickButtonPressed,
        InputEvent_JoystickButtonReleased,
    };

    enum EInputCategory : char
    {
        None = 0u,
        InputCategory_Keyboard = 1u << 0,
        InputCategory_Mouse = 1u << 1,
        InputCategory_Joystick = 1u << 2
    };

    class InputEvent : public Event
    {
    public:
        virtual EInputEvent GetEventType() const = 0;

        inline bool IsInCategory( EInputCategory category )
        {
            return GetCategoryFlags() & category;
        }
    };

    // Key Events //

    const class KeyEvent : public InputEvent
    {
    public:
        const int keyCode;
        const bool modKeyShift, modKeyCtrl, modKeyAlt, modKeySuper;
        
        FORCEINLINE bool GetKeyModifierSuper() const { return modKeySuper; };

        EE_IMPLEMENT_EVENT_CATEGORY( InputCategory_Keyboard )

    protected:
        KeyEvent( int code, bool shift, bool ctrl, bool alt, bool super )
            : keyCode( code ), modKeyShift( shift ), modKeyCtrl( ctrl ), modKeyAlt( alt ), modKeySuper( super )
        {
        }
    };

    const class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent( int code, bool shift, bool ctrl, bool alt, bool super, bool isRepeated )
            : KeyEvent( code, shift, ctrl, alt, super ), isRepeated( isRepeated )
        {
        }

        //* The key event was fired by maintaining the key pressed?
        const bool isRepeated;

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_KeyPress )
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent( int code, bool shift, bool ctrl, bool alt, bool super )
            : KeyEvent( code, shift, ctrl, alt, super )
        {
        }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_KeyRelease )
    };

    const class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent( const NChar text[ 32 ] )
            : KeyEvent( 0, false, false, false, false ), eventText( text )
        {
        }
        const NChar* eventText;

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_KeyType )
    };

    // Mouse Events //

    const class MouseEvent : public InputEvent
    {
    public:
        EE_IMPLEMENT_EVENT_CATEGORY( InputCategory_Mouse )

    protected:
        MouseEvent() {}
    };


    const class MouseMovedEvent : public MouseEvent
    {
    public:
        MouseMovedEvent( float x, float y, float relativeX, float relativeY )
            : mouseX( x ), mouseY( y ), mouseRelativeX( relativeX ), mouseRelativeY( relativeY )
        {
        }
        const float mouseX, mouseY;
        const float mouseRelativeX, mouseRelativeY;

        FORCEINLINE Point2 GetMousePosition() const { return { mouseX, mouseY }; }
        FORCEINLINE Point2 GetMouseRelativeMotion() const { return { mouseRelativeX, mouseRelativeY }; }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_MouseMoved )
    };

    const class MouseScrolledEvent : public MouseEvent
    {
    public:
        MouseScrolledEvent( float offsetX, float offsetY, bool isFlipped )
            : offsetX( offsetX ), offsetY( offsetY ), isFlipped( isFlipped )
        {
        }
        const float offsetX, offsetY;
        const bool isFlipped;

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_MouseScrolled )
    };

    const class MouseButtonEvent : public MouseEvent
    {
    public:
        const int eventButton;
        const bool isDoubleClick;
        const int repeatCount;
        
    protected:
        MouseButtonEvent( int button, bool isDoubleClick, int repeatCount )
            : eventButton( button ), isDoubleClick( isDoubleClick ), repeatCount( repeatCount )
        {
        }
    };

    const class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent( int button, bool doubleClick, int repeatCount )
            : MouseButtonEvent( button, doubleClick, repeatCount )
        {
        }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_MousePressed )
    };

    const class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent( int button )
            : MouseButtonEvent( button, false, 0 )
        {
        }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_MouseReleased )
    };

    // Joystick Event

    const class JoystickEvent : public InputEvent
    {
    public:
        const int joystickID;
        
        EE_IMPLEMENT_EVENT_CATEGORY( InputCategory_Joystick )

    protected:
        JoystickEvent( int joystickID )
            : joystickID( joystickID )
        {
        }
    };

    class JoystickConnectionEvent : public JoystickEvent
    {
    public:
        const int isConnected;

        JoystickConnectionEvent( int joystickID, int isConnected )
            : JoystickEvent( joystickID ), isConnected( isConnected )
        {
        }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_JoystickConnection )
    };

    class JoystickButtonPressedEvent : public JoystickEvent
    {
    public:
        const EGamepadButton buttonCode;

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_JoystickButtonPressed )

            JoystickButtonPressedEvent( int joystickID, EGamepadButton buttonCode )
            : JoystickEvent( joystickID ), buttonCode( buttonCode )
        {
        }
    };

    class JoystickButtonReleaseEvent : public JoystickEvent
    {
    public:
        const EGamepadButton buttonCode;
        
        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_JoystickButtonReleased )

            JoystickButtonReleaseEvent( int joystickID, EGamepadButton buttonCode )
            : JoystickEvent( joystickID ), buttonCode( buttonCode )
        {
        }
    };

    class JoystickAxisEvent : public JoystickEvent
    {
    public:
        const EGamepadAxis axis;
        const float value;

        JoystickAxisEvent( int joystickID, EGamepadAxis axis, float value )
            : JoystickEvent( joystickID ), axis( axis ), value( value )
        {
        }

        EE_IMPLEMENT_EVENT_ENUMTYPE( EInputEvent, InputEvent_JoystickAxis )

            EGamepadAxis GetAxis() const { return axis; };
    };
}