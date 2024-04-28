#include "tgpch.h"
#include "Renderer.h"
#include <wrl.h>

#include "Camera.h"
#include "Commands/CameraGFXCommand.h"
#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"
#include "CommonUtilities/Time/Time.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Primitives/Shader.h"
#include "Primitives/Vertex.h"
#include "Program/DepthProgram.h"
#include "RHI/PipelineStateObject.h"
#include "RHI/RHI.h"
#include "Tupla/AssetManager/AssetManager.h"
#include "Tupla/Core/Application.h"

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
        myDefaultPSO.RenderTargets.push_back(CreateRef<Texture>());    // BackBuffer
        myDefaultPSO.DepthStencil = CreateRef<Texture>();                   // DepthBuffer

        RHI::Init(*myWindow, myDefaultPSO.RenderTargets[0].get(), myDefaultPSO.DepthStencil.get(), true);
        CreateDefaultPSO();

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
        SwapBuffers();

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
                RHI::ResizeDevice(myDefaultPSO.RenderTargets[0], myDefaultPSO.DepthStencil);
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

	void Renderer::ProcessCommands()
	{
        ResetToDefault();
        Camera::Bind();

        for (const auto& camCmd : myRenderBuffer->GetVector<CameraGFXCommand>().GetData())
        {
            camCmd.CameraObject.Update();

            auto colorTarget = myDefaultPSO.RenderTargets[0].get();
            auto depthTarget = myDefaultPSO.DepthStencil.get();

            if(camCmd.ColorTarget != nullptr || camCmd.DepthTarget != nullptr) // This means target is our SwapChain!!!
            {
                colorTarget = camCmd.ColorTarget ? camCmd.ColorTarget.get() : nullptr;
                depthTarget = camCmd.DepthTarget ? camCmd.DepthTarget.get() : nullptr;
            }

            RunDepthPass(myRenderBuffer->GetVector<ModelGFXCommand>(), camCmd.CameraObject, myDefaultVertexShader, nullptr, colorTarget);
        }


        RHI::SetRenderTarget(myDefaultPSO.RenderTargets[0].get(), myDefaultPSO.DepthStencil.get());
	}

	void Renderer::EndFrame()
	{
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

	void Renderer::CreateDefaultPSO()
	{
        std::string defaultShaderPath = Application::Get().GetAssetManager().GetAssetPath("Internal/Default.hlsl");

        myDefaultVertexShader = CreateRef<Shader>();
        myDefaultPixelShader = CreateRef<Shader>();

        ComPtr<ID3DBlob> compiledCode {};

        if (!myDefaultVertexShader->CompileShader(defaultShaderPath, ShaderType::VertexShader, {}, false, &compiledCode))
        {
            LOG_CRITICAL("Failed to compile default vertex shader...");
        }

		if(!myDefaultPixelShader->CompileShader(defaultShaderPath, ShaderType::PixelShader, {}, true))
		{
            LOG_ERROR("Failed to compile default pixel shader...");
		}

        RHI::GetDepthState(myDefaultPSO.DepthStencilState, DepthState::Default);
        RHI::GetRasterizerState(myDefaultPSO.RasterizerState, RasterizerState::CullBack);
        RHI::CreateBlendState(myDefaultPSO.BlendState, {});
        RHI::CreateInputLayout(myDefaultPSO.InputLayout, Vertex::InputLayoutDefinition, compiledCode->GetBufferPointer(), compiledCode->GetBufferSize());

        myDefaultPSO.VertexShader = myDefaultVertexShader;
        myDefaultPSO.PixelShader = myDefaultPixelShader;

        myDefaultPSO.ClearRenderTargets = true;
        myDefaultPSO.ClearDepthStencil = true;
	}

	void Renderer::ResetToDefault()
	{
        RHI::SetPipelineStateObject(&myDefaultPSO);
	}
}
