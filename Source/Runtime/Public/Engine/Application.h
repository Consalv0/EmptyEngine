#pragma once

namespace EE
{
    class Application
    {
    protected:
        bool canInitialize;

    public:
        //* Entry point of the application
        void Start();

        //* Application loop
        void MainLoop();

        virtual ~Application();

        virtual enum class EDynamicRHI GetPreferedRHI() const = 0;

        bool CanInitialize() { return canInitialize; };

    protected:
        Application();

        //* Called after creating the main window
        virtual bool OnInitialize() = 0;

        //* Called after Initialize
        virtual void OnAwake() = 0;

        virtual void OnProcessInput() = 0;

        //* This is called after pulling all events
        virtual void OnTick( Timestamp stamp ) = 0;

        //* Called when app is asked to terminate
        virtual void OnTerminate() = 0;

    private:
        bool initialized;

        //* Initialize the application objects
        bool Initialize();

        //* Application loading point
        void Awake();

        //* Terminates Application
        void Terminate();
    };

    extern Application* GMainApplication;

    Application* CreateApplication( int argc, NChar* argv[] );
}