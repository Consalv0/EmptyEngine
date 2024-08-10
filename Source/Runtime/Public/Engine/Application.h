#pragma once

namespace EE
{
    class Application
    {
    protected:
        bool canInitialize;

    public:
        class RenderPipeline& GetRenderPipeline();

        //* Entry point of the application
        void Run();

        virtual ~Application();

        virtual enum class EDynamicRHI GetPreferedRHI() const = 0;

        bool CanInitialize() { return canInitialize; };

    protected:

        Application();

        virtual bool OnInitialize() = 0;

        virtual void OnAwake() {};

        //* This is called after pulling all events,
        //* you need to update the input manager here
        virtual void OnUpdate( Timestamp stamp ) {};

        virtual void OnTerminate();

        virtual void OnDestroy() {};

    private:
        bool initialized;

        //* Initialize the application objects
        bool Initialize();

        //* Application loading point
        void Awake();

        //* Application loop
        void UpdateLoop();

        //* Terminates Application
        void Terminate();
    };

    extern Application* GMainApplication;

    Application* CreateApplication( int argc, char** argv );
}