#pragma once

#include "Events/Event.h"

namespace EE 
{
    enum class EEventWindow 
    {
        EventWindow_Close,
        EventWindow_Resize,
        EventWindow_GainFocus,
        EventWindow_LostFocus,
        EventWindow_Moved
    };

#ifndef IGNORE_EVENT
    const class WindowEvent : public Event
    {
    public:
        virtual EEventWindow GetEventType() const = 0;

        EE_IMPLEMENT_EVENT_CATEGORY(0u);

    protected:
        WindowEvent() {};
    };

    const class WindowCloseEvent : public WindowEvent
    {
    public:
        WindowCloseEvent() {}

        EE_IMPLEMENT_EVENT_ENUMTYPE( EEventWindow, EventWindow_Close );
    };

    const class WindowResizeEvent : public WindowEvent
    {
    public:
        WindowResizeEvent(uint32 width, uint32 height)
            : width(width), height(height) {}

        EE_IMPLEMENT_EVENT_ENUMTYPE( EEventWindow, EventWindow_Resize );

    public:
        const uint32 width, height;
    };

    class WindowGainFocusEvent : public WindowEvent
    {
    public:
        WindowGainFocusEvent() {}

        EE_IMPLEMENT_EVENT_ENUMTYPE(EEventWindow, EventWindow_GainFocus );
    };

    class WindowLostFocusEvent : public WindowEvent
    {
    public:
        WindowLostFocusEvent() {};

        EE_IMPLEMENT_EVENT_ENUMTYPE(EEventWindow, EventWindow_LostFocus );
    };

    class WindowMovedEvent : public WindowEvent
    {
    public:
        WindowMovedEvent( uint32 x, uint32 y) : x( x ), y( y ) {}

        EE_IMPLEMENT_EVENT_ENUMTYPE( EEventWindow, EventWindow_Moved );

    public:
        const uint32 x, y;
    };
#endif
}