#include "tgpch.h"
#include "DX11Renderer.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

namespace Tupla
{
	void DX11Renderer::StartWindow(const WindowProps& props)
	{
		m_Window = std::make_unique<WindowsWindow>(props);
	}

	bool DX11Renderer::Init()
	{
        CreateSwapChain();
        CreateFrameBuffer();
        CreateDepthBuffer();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;      // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;          // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;        // Enable Multi-Viewport / Platform windows

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        m_ViewportTexture = std::make_unique<DX11RenderTexture>(this, 128u, 128u, true);
        ImGui_ImplWin32_Init(m_Window->GetWindowHandle());
        ImGui_ImplDX11_Init(m_Device, m_Context);
        return true;
	}

	void DX11Renderer::Shutdown()
	{
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::Shutdown();
	}

	void DX11Renderer::BeginFrame()
	{
        m_Window->PollEvents();

        // Recreate viewport texture to match the viewport size
        if(m_ViewportTexture)
        {
            if(m_ViewportSize.x == 0)
            {
                m_ViewportSize.x = 1;
            }

            if (m_ViewportSize.y == 0)
            {
                m_ViewportSize.y = 1;
            }

            if (m_ViewportSize.x != m_ViewportTexture->m_Width || m_ViewportSize.y != m_ViewportTexture->m_Height)
            {
                m_ViewportTexture = std::make_unique<DX11RenderTexture>(this, m_ViewportSize.x, m_ViewportSize.y, m_ViewportTexture->m_UsesDepth);
            }
        }

        FLOAT clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
        m_Context->ClearRenderTargetView(m_FrameBufferRTV, clearColor);
        m_Context->ClearDepthStencilView(m_DepthBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        SetRenderTexture(m_ViewportTexture.get());
        m_ViewportTexture->Clear({ 0.0555f, 0.1882f, 0.3882f, 1 });
        m_Context->OMSetDepthStencilState(m_DepthStencilState, 0);
        m_Context->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. no blending)

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

	void DX11Renderer::EndFrame()
	{
        m_Context->OMSetRenderTargets(1, &m_FrameBufferRTV, m_DepthBufferDSV);
        ImGuiIO& io = ImGui::GetIO();
        // Rendering
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        auto result = m_SwapChain->Present(1, 0);
        ASSERT(SUCCEEDED(result), "Failed to present rendered frame!")
	}

	void* DX11Renderer::GetViewportImage(const CU::Vector2ui& viewportSize)
	{
        m_ViewportSize = viewportSize;
        return m_ViewportTexture ? m_ViewportTexture->ImGuiRID() : nullptr;
	}

	void DX11Renderer::SetRenderTexture(const DX11RenderTexture* renderTexture) const
	{
        if(renderTexture)
        {
            m_Context->OMSetRenderTargets(1, &renderTexture->m_TextureRTV, renderTexture->m_DepthDSV);
        }
        else
        {
            m_Context->OMSetRenderTargets(1, &m_FrameBufferRTV, m_DepthBufferDSV);
        }
	}

	void DX11Renderer::CreateSwapChain()
	{
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
        swapchaindesc.BufferDesc.Width = 0; // use window width
        swapchaindesc.BufferDesc.Height = 0; // use window height
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapchaindesc.SampleDesc.Count = 1;
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchaindesc.BufferCount = 2;
        swapchaindesc.OutputWindow = (HWND)m_Window->GetWindowHandle();
        swapchaindesc.Windowed = TRUE;
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            featurelevels,
            ARRAYSIZE(featurelevels),
            D3D11_SDK_VERSION,
            &swapchaindesc,
            &m_SwapChain,
            &m_Device,
            nullptr,
            &m_Context
        );
	}

	void DX11Renderer::CreateFrameBuffer()
	{
        auto result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_FrameBuffer);
        ASSERT(SUCCEEDED(result), "Failed to get framebuffer from swapchain");

        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {}; // needed for SRGB framebuffer when using FLIP model swap effect
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        result = m_Device->CreateRenderTargetView(m_FrameBuffer, &framebufferRTVdesc, &m_FrameBufferRTV);
        ASSERT(SUCCEEDED(result), "Failed to create RenderTargetView from framebuffer");
	}

	void DX11Renderer::CreateDepthBuffer()
	{
        D3D11_TEXTURE2D_DESC depthbufferdesc;

		m_FrameBuffer->GetDesc(&depthbufferdesc);

        depthbufferdesc.Format      = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthbufferdesc.BindFlags   = D3D11_BIND_DEPTH_STENCIL;

        auto result = m_Device->CreateTexture2D(&depthbufferdesc, nullptr, &m_DepthBuffer);
        ASSERT(SUCCEEDED(result), "Failed to create depth texture");

        result = m_Device->CreateDepthStencilView(m_DepthBuffer, nullptr, &m_DepthBufferDSV);
        ASSERT(SUCCEEDED(result), "Failed to create depth stencil view");
	}

	void DX11Renderer::CreateStates()
	{
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID;
        rasterizerdesc.CullMode = D3D11_CULL_BACK;

        m_Device->CreateRasterizerState(&rasterizerdesc, &m_RasterizerState);

        D3D11_SAMPLER_DESC samplerdesc = {};
        samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        m_Device->CreateSamplerState(&samplerdesc, &m_SamplerState);

        D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
        depthstencildesc.DepthEnable = TRUE;
        depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;

        m_Device->CreateDepthStencilState(&depthstencildesc, &m_DepthStencilState);
	}
}
