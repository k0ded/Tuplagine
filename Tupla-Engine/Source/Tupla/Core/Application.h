#pragma once
#include <cassert>

#include "Layer.h"
#include "LayerStack.h"
#include "Window.h"
#include "Tupla/Core/Base.h"
#include "Tupla/Renderer/Renderer.h"
#include <Tupla/AssetManager/AssetManager.h>

#include "Tupla/Jobs/JobManager.h"

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
        std::wstring ApplicationName = L"May the Tupla be with you... Always.";
        std::string WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;
    };
    
    class Application
    {
    public:
        Application(ApplicationSpecification specification);
        virtual ~Application();

        // Application takes ownership of layers that stay on until the applications end.
        void Push(Layer* layer);
        void PushOverlay(Layer* layer);
        
    	ApplicationSpecification& GetSpecification() { return m_Specification; }
        void Shutdown(bool restarting = false);

        Renderer* GetRenderer() const { return m_Renderer.get(); }
        AssetManager& GetAssetManager() const { return *m_AssetManager; }
        JobSystem::JobManager& GetJobManager() { return m_JobManager; }

        static Application& Get() { return *s_Application; }

    	inline static bool m_Restarting = false;
    private:
        void Run();

        Scope<Renderer> m_Renderer = nullptr;
        Scope<AssetManager> m_AssetManager {};
        
        ApplicationSpecification m_Specification;
        JobSystem::JobManager m_JobManager;
        LayerStack m_LayerStack;
        bool m_Running;

        std::atomic<float> m_progress;
        
        static inline Application* s_Application = nullptr;
        friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication(ApplicationCommandLineArgs args);
}
