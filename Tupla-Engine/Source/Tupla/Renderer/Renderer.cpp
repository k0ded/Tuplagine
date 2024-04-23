#include "tgpch.h"
#include "Renderer.h"
#include <wrl.h>

#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"
#include "CommonUtilities/Time/Time.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "RHI/PipelineStateObject.h"

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")

using namespace Microsoft::WRL;

namespace Tupla
{
	void Renderer::StartWindow(const WindowProps& props)
	{
		myWindow = std::make_unique<WindowsWindow>(props);
	}

	bool Renderer::Init()
	{
        myBackBuffer = CreateScope<Texture>();
        myDepthBuffer = CreateScope<Texture>();

        RHI::Init(*myWindow, myBackBuffer.get(), myDepthBuffer.get(), true);

        ImGui::CreateContext();
        ImGui_ImplDX11_Init(RHI::GetDevice().Get(), RHI::GetContext().Get());
        ImGui_ImplWin32_Init(myWindow->GetWindowHandle());

        return true;
	}

	void Renderer::Shutdown()
	{
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
	}

	void Renderer::BeginFrame()
	{
        myFrameWatch.Update();
        myPrevTimes.push_back(myFrameWatch.GetDeltaTime());

        if(myPrevTimes.size() > 10)
        {
            myPrevTimes.erase(myPrevTimes.begin());
        }

        float stabilized = 0.0f;

        for (const float prevTime : myPrevTimes)
        {
            stabilized += prevTime;
        }

        stabilized /= myPrevTimes.size();

        myWindow->SetTitle(L"FPS: " + std::to_wstring(1.f / stabilized));
        myWindow->PollEvents();

        // Resize buffers if window size isn't correct
        {
	        const auto windowExtents = myWindow->GetExtents();
            if (windowExtents != myRenderingSize)
            {
                myRenderingSize = windowExtents;
                myBackBuffer = CreateScope<Texture>();
                myDepthBuffer = CreateScope<Texture>();
                RHI::ResizeDevice(myBackBuffer.get(), myDepthBuffer.get());
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

	void Renderer::EndFrame()
	{
        RHI::SetRenderTarget(myBackBuffer.get(), myDepthBuffer.get());
        ImGuiIO& io = ImGui::GetIO();
        // Rendering IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        RHI::Present();
	}
}
