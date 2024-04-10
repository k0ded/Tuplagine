#pragma once
#include <d3d11.h>
#include <dxgi.h>

#include "WindowsWindow.h"
#include "CommonUtilities/Time/StopWatch.h"
#include "Primitives/DX11RenderTexture.h"
#include "Tupla/Renderer/Material.h"
#include "Tupla/Renderer/Renderer.h"

#define BUFFER_PERFORMANCE_WARN 1

namespace Tupla
{
	class DX11Renderer : public Renderer
	{
	public:
		void StartWindow(const WindowProps& props) override;
		bool Init() override;
		void Shutdown() override;

		void BeginFrame() override;
		void EndFrame() override;
		
		Window* GetWindow() override { return m_Window.get(); }

		ID3D11Device* GetDevice() const { return m_Device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return m_Context.Get(); }

		void RenderMesh(Ref<Mesh> mesh, Ref<Material> material) override;

		void* GetViewportImage(const CU::Vector2ui& viewportSize) override;
		CU::Vector2ui GetViewportSize() override { return m_ViewportSize; }

		void SetRenderTexture(const DX11RenderTexture* renderTexture = nullptr) const;

		static void SetObjectName(ID3D11DeviceChild* aObject, const char* aName);

		CLASS_RENDER_IMPL(DX11)
	private:
		void CreateSwapChain();
		void ResizeSwapChain();
		void CreateFrameBuffer();
		void CreateDepthBuffer();
		void CreateStates();

		Scope<WindowsWindow> m_Window;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;

		Scope<DX11RenderTexture> m_SwapChainRenderTexture;

		// Defaults
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
		D3D11_VIEWPORT m_DXViewport;

		Scope<DX11RenderTexture> m_ViewportTexture;

		CU::Vector2ui m_ViewportSize;
		CU::Vector2ui m_RenderingSize;

		CommonUtilities::StopWatch m_FrameWatch{};
		std::vector<float> m_PrevTimes{};
	};
}
