#include "tgpch.h"
#include "Application.h"

#include <filesystem>

#include <CommonUtilities/Time/Time.h>

#include "Tupla/Renderer/Vulkan/VulkanRenderer.h"

namespace Tupla
{
    Application::Application(ApplicationSpecification specification): m_Specification(std::move(specification))
    {
        m_Restarting = false;
        s_Application = this;
        m_Renderer = CreateScope<VulkanRenderer>();

        if(!m_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(m_Specification.WorkingDirectory);

        m_Window = m_Renderer->StartWindow({ m_Specification.ApplicationName });
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
        while (m_Running)
        {
            Time::Update();

            m_Renderer->BeginFrame();
            if(!m_Window->GetMinimized())
            {
                {
                    for (const auto layer : m_LayerStack)
                    {
                        layer->OnUpdate();
                    }
                }

                m_Renderer->BeginGUIFrame();
                {
                    for (const auto layer : m_LayerStack)
                    {
                        layer->OnGUI();
                    }
                }
                m_Renderer->EndGUIFrame();
            }
            
            m_Renderer->EndFrame();

            m_Running = !m_Window->ShouldClose();
        }
    }
}
