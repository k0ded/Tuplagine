#pragma once
#include <d3d11.h>
#include <dxgi.h>

#include "WindowsWindow.h"
#include "Primitives/DX11RenderTexture.h"
#include "Primitives/DX11Shader.h"
#include "Tupla/Renderer/Renderer.h"

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

		ID3D11Device* GetDevice() const { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() const { return m_Context; }

		void* GetViewportImage(const CU::Vector2ui& viewportSize) override;

		void SetRenderTexture(const DX11RenderTexture* renderTexture = nullptr) const;

		CLASS_RENDER_IMPL(DX11)
	private:
		void CreateSwapChain();
		void CreateFrameBuffer();
		void CreateDepthBuffer();
		void CreateStates();

		Scope<WindowsWindow> m_Window;

		IDXGISwapChain* m_SwapChain;
		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_Context;

		ID3D11Texture2D* m_FrameBuffer;
		ID3D11RenderTargetView* m_FrameBufferRTV;
		ID3D11Texture2D* m_DepthBuffer;
		ID3D11DepthStencilView* m_DepthBufferDSV;

		// Defaults
		ID3D11RasterizerState* m_RasterizerState;
		ID3D11SamplerState* m_SamplerState;
		ID3D11DepthStencilState* m_DepthStencilState;

		Scope<DX11RenderTexture> m_ViewportTexture;

		CU::Vector2ui m_ViewportSize;
	};
}
