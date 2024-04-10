#include "tgpch.h"
#include "Application.h"

#include <filesystem>

#include <CommonUtilities/Time/Time.h>

#include "CommonUtilities/Input/Input.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
    Application::Application(ApplicationSpecification specification): m_Specification(std::move(specification))
    {
        m_Running = false;
        m_Restarting = false;
        s_Application = this;
        Logger::Init();
        
        m_AssetManager = CreateScope<AssetManager>();
        m_Renderer = CreateScope<DX11Renderer>();
        m_Renderer->StartWindow({ m_Specification.ApplicationName });
        m_Renderer->Init();

        if (!m_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(m_Specification.WorkingDirectory);

        // EXAMPLE OF HOW TO MAKE ASYNC JOBS!
        // m_JobManager.ScheduleJob(Job([this] {
        // 	while (m_progress < 100.f)
	    //     {
        //         std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //         m_progress += 0.105f;
	    //     }
        // }));
        // m_JobManager.ScheduleJob(Job([this] { while (m_progress < 100.f) { std::cout << m_progress << '\n'; }}));
        // m_JobManager.BurstJobs(); // DISPATCHES THE JOBS AND CONTINUES EXECUTION
        // m_JobManager.Await(); // DISPATCHES THE JOBS AND BLOCKS EXECUTION UNTIL FINISHED
    }

    Application::~Application()
    {
        m_Renderer->Shutdown();
        m_Renderer = nullptr;
    }

    void Application::Push(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlayLayer(layer);
    }

    void Application::Shutdown(bool restarting)
    {
        m_Running = false;
        m_Restarting = restarting;
    }

    void Application::Run()
    {
        m_Running = true;
        m_Restarting = false;
        while (m_Running)
        {
            Time::Update();
            Input::Update();

            m_Renderer->BeginFrame();
            {
                for (const auto layer : m_LayerStack)
                {
                    layer->OnUpdate();
                }
            }

            {
                for (const auto layer : m_LayerStack)
                {
                    layer->OnGUI();
                }
            }
            
            m_Renderer->EndFrame();

            m_Running = !m_Renderer->GetWindow()->ShouldClose() && m_Running;
        }
    }
}
