#pragma once
#include <vulkan/vulkan_core.h>

#include "Tupla/Core/Window.h"

namespace Tupla
{
    class VulkanWindow final : public Window
    {
    public:
        VulkanWindow(const WindowProps& props);
        ~VulkanWindow() override;
        
        void SetTitle(const std::string& newTitle) override;
        bool ShouldClose() override;
        bool GetMinimized() override;
        
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;
        bool WasWindowResized() const;
        void ResetWindowResizeFlag();

    private:
        bool m_WindowResizedFlag;
        bool m_WindowMinimizedFlag;
    };
}
