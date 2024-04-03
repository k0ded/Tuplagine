#include "tgpch.h"
#include "Application.h"

#include <filesystem>

#include <CommonUtilities/Time/Time.h>

#include "imgui/imgui.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
    Application::Application(ApplicationSpecification specification): m_Specification(std::move(specification))
    {
        m_Restarting = false;
        s_Application = this;
        Logger::Init();
        
        m_AssetManager = CreateScope<AssetManager>();
        m_Renderer = CreateScope<DX11Renderer>();
        m_Renderer->StartWindow({ m_Specification.ApplicationName });
        m_Renderer->Init();

        if(!m_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(m_Specification.WorkingDirectory);

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
        m_Renderer->Shutdown();
    }

    void Application::Run()
    {
        m_Running = true;
        while (m_Running)
        {
            Time::Update();

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

            m_Running = !m_Renderer->GetWindow()->ShouldClose();
        }
    }
}
