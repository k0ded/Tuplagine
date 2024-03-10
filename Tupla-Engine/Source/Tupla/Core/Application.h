#pragma once
#include <cassert>

#include "Layer.h"
#include "LayerStack.h"
#include "Window.h"
#include "Tupla/Core/Base.h"
#include "Tupla/Renderer/Renderer.h"

int main(int argc, char** argv);

namespace Tupla
{
    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](int index) const
        {
            assert(index < Count);
            return Args[index];
        }
    };
    
    struct ApplicationSpecification
    {
        std::string ApplicationName = "May the Tupla be with you... Always.";
        std::string WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;
    };
    
    class Application
    {
    public:
        Application(ApplicationSpecification specification);
        virtual ~Application() = default;

        // Application takes ownership of layers that stay on until the applications end.
        void Push(Layer* layer);
        void PushOverlay(Layer* layer);
        
        const ApplicationSpecification& GetSpecification() { return m_Specification; }
        void Shutdown(bool restarting = false);

        Renderer* GetRenderer() const { return m_Renderer.get(); }

        static Application& Get() { return *s_Application; }
    private:
        void Run();

        Scope<Renderer> m_Renderer;
        Scope<Window> m_Window;
        
        ApplicationSpecification m_Specification;
        LayerStack m_LayerStack;
        bool m_Running;
        bool m_Restarting;
        
        static inline Application* s_Application = nullptr;
        friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication(ApplicationCommandLineArgs args);
}
