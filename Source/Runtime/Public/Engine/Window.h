#pragma once

#include "CoreMinimal.h"
#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsDevice.h"

union WindowEventData;

namespace EE
{
    enum EWindowMode
    {
        WindowMode_Windowed = 0,
        WindowMode_FullScreen = 1
    };

    template <typename T>
    class Bitmap;

    struct WindowProperties
    {
        //* Name displayed in header window
        WString name;
        uint32_t width;
        uint32_t height;
        EWindowMode windowMode;

        WindowProperties(
            const WString& title = L"Empty Engine",
            uint32_t width = 1280,
            uint32_t height = 720,
            EWindowMode mode = WindowMode_Windowed )
            : name(title), width(width), height(height), windowMode(mode) {
        }

    };

    //* Cointains the properties and functions of a window
    class Window
    {
    protected:
        EWindowMode mode_;

        void* windowHandle_;

        std::unique_ptr<GraphicsDevice> device_;

        SwapChain swapChain_;

    public:

        virtual ~Window() = default;

        //* Begin of frame functions
        virtual void BeginFrame() = 0;

        //* End of frame functions
        virtual void EndFrame() = 0;

        //* Set the window display mode
        virtual void SetWindowMode(EWindowMode mode) = 0;

        //* Get the window display mode
        virtual EWindowMode GetWindowMode() const = 0;

        //* Get the window title name
        virtual WString GetName() const = 0;

        //* Get the width in pixels of the window
        virtual int GetWidth() const = 0;

        //* Get the height in pixels of the window
        virtual int GetHeight() const = 0;

        //* Get the size of the window in pixels
        virtual IntVector2 GetSize() const = 0;

        //* Get the size of the window in pixels
        virtual IntBox2D GetViewport() const = 0;

        //* Get the total frames renderized
        virtual uint64_t GetFrameCount() const = 0;

        //* Get the aspect of width divided by height in pixels of the window
        virtual float GetAspectRatio() const = 0;

        //* Get SDL_Window Pointer
        virtual void* GetHandle() const = 0;

        //* Creates a Window with a Name, Width and Height
        static Window* Create(const WindowProperties& parameters = WindowProperties());

    public:
        virtual FORCEINLINE GraphicsDevice& GetDevice() const { return *device_; };
    };

}
