#include "tgpch.h"
#include "VulkanWindow.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Tupla
{
    VulkanWindow::VulkanWindow(const WindowProps& props): Window(props)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        
        m_Handle = glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_Handle), this);

        m_WindowMinimizedFlag = false;

        glfwSetWindowSizeCallback(static_cast<GLFWwindow*>(m_Handle), [](GLFWwindow* window, int w, int h)
        {
            static_cast<VulkanWindow*>(glfwGetWindowUserPointer(window))->m_WindowResizedFlag = true;
            static_cast<VulkanWindow*>(glfwGetWindowUserPointer(window))->m_WindowMinimizedFlag = w > 0 && h > 0;
        });
        // TODO: SETUP EVENTS!!!
    }

    VulkanWindow::~VulkanWindow()
    {
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_Handle));
        glfwTerminate();
    }

    void VulkanWindow::SetTitle(const std::string& newTitle)
    {
        glfwSetWindowTitle(static_cast<GLFWwindow*>(m_Handle), newTitle.c_str());
    }

    bool VulkanWindow::ShouldClose()
    {
        return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_Handle));
    }

    bool VulkanWindow::GetMinimized()
    {
        return m_WindowMinimizedFlag;
    }

    void VulkanWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
    {
        if(glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(m_Handle), nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool VulkanWindow::WasWindowResized() const
    {
        return m_WindowResizedFlag;
    }

    void VulkanWindow::ResetWindowResizeFlag()
    {
        m_WindowResizedFlag = false;
    }
}
