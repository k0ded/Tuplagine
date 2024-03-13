#pragma once
#include "Tupla/Core/Base.h"
#include "Tupla/Renderer/Vulkan/VulkanWindow.h"
#include <imgui/imgui_impl_vulkan.h>
#include "Tupla/Renderer/Renderer.h"

namespace Tupla 
{
	class VulkanRenderer : public Renderer
	{
	public:
		bool Init() override;
		void Shutdown() override;

		void StartWindow(const WindowProps& props) override;

		void BeginFrame() override;
		void EndFrame() override;

		Window* GetWindow() override;

		CLASS_RENDER_IMPL(VULKAN)
	private:
		Scope<VulkanWindow> m_Window;
		ImGui_ImplVulkanH_Window* m_Wd;
	};
}
