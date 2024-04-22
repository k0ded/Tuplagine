#include "tgpch.h"
//#include "RenderHardwareInterface.h"
//#include <d3d11.h>
//#include <d3d11_1.h>
//
//#include <d3dcompiler.h>
//#include <d3d11shader.h>
//
//#include "Tupla/Core/Log.h"
//#include "CommonUtilities/Strings.hpp"
//
//#include "Texture.h"
//#include "Shader.h"
//#include "ConstantBuffer.h"
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_dx11.h"
//#include "imgui/imgui_impl_win32.h"
//
//using namespace Microsoft::WRL;
//
//namespace Tupla
//{
//	RenderHardwareInterface::RenderHardwareInterface() = default;
//	RenderHardwareInterface::~RenderHardwareInterface() = default;
//
//	bool RenderHardwareInterface::Initialize(HWND aWindowHandle, bool aEnableDebug)
//	{
//		HRESULT result = E_FAIL;
//
//		ComPtr<IDXGIFactory> dxFactory;
//		result = CreateDXGIFactory(__uuidof(IDXGIFactory), &dxFactory);
//
//		if (FAILED(result))
//		{	
//			LOG_CRITICAL("Failed to create DX Factory!");
//			return false;
//		}
//
//		LOG_INFO("Initializing RHI...");
//
//		result = dxFactory->MakeWindowAssociation(aWindowHandle, 0);
//
//		if(FAILED(result))
//		{
//			LOG_ERROR("Failed to make window association!");
//			return false;
//		}
//
//		ComPtr<IDXGIAdapter> tempAdapter;
//		std::vector<ComPtr<IDXGIAdapter>> adapters;
//		unsigned adapterCount = 0;
//		while (dxFactory->EnumAdapters(adapterCount, &tempAdapter) != DXGI_ERROR_NOT_FOUND)
//		{
//			adapters.push_back(tempAdapter);
//			adapterCount++;
//		}
//
//		ComPtr<IDXGIAdapter> selectedAdapter;
//		size_t selectedAdapterVRAM = 0;
//		DXGI_ADAPTER_DESC selectedAdapterDesc = {};
//		for (const auto& adapter : adapters)
//		{
//			DXGI_ADAPTER_DESC currentDesc = {};
//			adapter->GetDesc(&currentDesc);
//
//			if (currentDesc.DedicatedVideoMemory > selectedAdapterVRAM)
//			{
//				selectedAdapterVRAM = currentDesc.DedicatedVideoMemory;
//				selectedAdapter = adapter;
//				selectedAdapterDesc = currentDesc;
//			}
//		}
//
//		const wchar_t* wideAdapterName = selectedAdapterDesc.Description;
//		const std::string adapterName = CU::string_cast<std::string>(wideAdapterName);
//		
//		LOG_INFO("Selected adapter is {}", adapterName);
//		constexpr size_t megabyte = (1024ULL * 1024ULL);
//		if (selectedAdapterVRAM > megabyte)
//		{
//			selectedAdapterVRAM /= megabyte;
//		}
//
//		LOG_INFO(" * VRAM: {}", selectedAdapterVRAM);
//
//		ComPtr<ID3D11Device> device;
//		ComPtr<ID3D11DeviceContext> context;
//
//		result = D3D11CreateDevice(
//			selectedAdapter.Get(),
//			D3D_DRIVER_TYPE_UNKNOWN,
//			NULL,
//			aEnableDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
//			NULL,
//			NULL,
//			D3D11_SDK_VERSION,
//			&device,
//			NULL,
//			&context
//		);
//
//		if (FAILED(result))
//		{
//			LOG_CRITICAL("Failed to initialize DirectX!");
//			return false;
//		}
//
//		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
//		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32/bit unsigned normalized RGBA
//		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		swapChainDesc.OutputWindow = aWindowHandle;
//		swapChainDesc.SampleDesc.Count = 1;
//		swapChainDesc.BufferCount = 2;
//		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
//		swapChainDesc.Windowed = true;
//
//		ComPtr<IDXGISwapChain> swapChain;
//		result = dxFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);
//
//		if (FAILED(result))
//		{
//			LOG_CRITICAL("Failed to create SwapChain!"); 
//			return false;
//		}
//
//		// Query if we support ID3DUserDefinedAnnotation
//		context->QueryInterface(IID_PPV_ARGS(&myUDA));
//
//		myDevice = device;
//		myContext = context;
//		mySwapChain = swapChain;
//
//		RECT clientRect;
//		GetClientRect(aWindowHandle, &clientRect);
//		const CU::Vector2ui clientSize = {
//			static_cast<u32>(clientRect.right - clientRect.left),
//			static_cast<u32>(clientRect.bottom - clientRect.top)
//		};
//		if(!CreateBackBuffer(clientSize))
//		{
//			return false;
//		}
//
//		if(!CreateDepthBuffer(clientSize))
//		{
//			return false;
//		}
//
//		SetObjectName(myContext, "Device Context");
//		SetObjectName(myBackBuffer->myRTV, "BackBuffer RTV");
//
//
//		LOG_INFO("Initializing IMGUI");
//		IMGUI_CHECKVERSION();
//		ImGui::CreateContext();
//
//		ImGuiIO& io = ImGui::GetIO(); (void)io;
//		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;      // Enable Keyboard Controls
//		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;          // Enable Docking
//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;        // Enable Multi-Viewport / Platform windows
//
//		ImGuiStyle& style = ImGui::GetStyle();
//		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//		{
//			style.WindowRounding = 0.0f;
//			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//		}
//
//		ImGui_ImplWin32_Init(aWindowHandle);
//		ImGui_ImplDX11_Init(myDevice.Get(), myContext.Get());
//
//		LOG_INFO("RHI Initialized!");
//
//		return true;
//	}
//
//	void RenderHardwareInterface::Destroy()
//	{
//		ImGui_ImplWin32_Shutdown();
//		ImGui_ImplDX11_Shutdown();
//		ImGui::DestroyContext();
//	}
//
//	void RenderHardwareInterface::Present() const
//	{
//		mySwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
//	}
//
//	bool RenderHardwareInterface::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
//	{
//		D3D11_BUFFER_DESC indexBufferDesc = {};
//		indexBufferDesc.ByteWidth = static_cast<unsigned>(aIndexList.size() * sizeof(unsigned));
//		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//
//		D3D11_SUBRESOURCE_DATA indexSubresoureceData = {};
//		indexSubresoureceData.pSysMem = aIndexList.data();
//
//		const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresoureceData, outIxBuffer.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create index buffer - {}.", aName);
//			return false;
//		}
//
//		SetObjectName(outIxBuffer, aName);
//
//		return true;
//	}
//
//	bool RenderHardwareInterface::CreateConstantBuffer(std::string_view aName, unsigned aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer)
//	{
//		if (aSize > 65536)
//		{
//			LOG_ERROR("Failed to create constant buffer {}. Size is larger than 64kb ({}).", aName, aSize);
//			return false;
//		}
//
//		outBuffer.myName = aName;
//		outBuffer.mySize = aSize;
//		outBuffer.myPipelineStages = aPipelineStages;
//		outBuffer.mySlotIndex = aSlot;
//
//		D3D11_BUFFER_DESC bufferDesc = {};
//		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		bufferDesc.ByteWidth = aSize;
//
//		const HRESULT result = myDevice->CreateBuffer(&bufferDesc, nullptr, outBuffer.myBuffer.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create constant buffer {}. Check DirectX log for more information.", aName);
//			return false;
//		}
//
//		SetObjectName(outBuffer.myBuffer, aName);
//		LOG_INFO("Created constant buffer {}.", aName);
//
//		return true;
//	}
//
//	void RenderHardwareInterface::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset) const
//	{
//		const unsigned vertexSize = static_cast<unsigned>(aVertexSize);
//		const unsigned vertexOffset = static_cast<unsigned>(aVertexOffset);
//
//		myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &vertexSize, &vertexOffset);
//	}
//
//	void RenderHardwareInterface::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const
//	{
//		const DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
//		myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
//	}
//
//	void RenderHardwareInterface::SetConstantBuffer(ConstantBuffer& aBuffer) const
//	{
//		if (aBuffer.myPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
//		{
//			myContext->VSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
//		}
//		if (aBuffer.myPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
//		{
//			myContext->PSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
//		}
//		if (aBuffer.myPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
//		{
//			myContext->GSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
//		}
//	}
//
//	void RenderHardwareInterface::SetPrimitiveTopology(unsigned aTopology) const
//	{
//		myContext->IASetPrimitiveTopology(static_cast<D3D10_PRIMITIVE_TOPOLOGY>(aTopology));
//	}
//
//	bool RenderHardwareInterface::CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
//	{
//		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
//		inputElements.reserve(aInputLayoutDefinition.size());
//
//		for (const auto& vxED : aInputLayoutDefinition)
//		{
//			D3D11_INPUT_ELEMENT_DESC element = {};
//			element.SemanticName = vxED.Semantic.data();
//			element.SemanticIndex = vxED.SemanticIndex;
//			element.Format = static_cast<DXGI_FORMAT>(vxED.Type);
//
//			element.InputSlot = 0;
//			element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
//			element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//			element.InstanceDataStepRate = 0;
//
//			inputElements.emplace_back(element);
//		}
//
//		const HRESULT result = myDevice->CreateInputLayout(
//			inputElements.data(),
//			static_cast<unsigned>(inputElements.size()),
//			aShaderDataPtr,
//			aShaderDataSize,
//			outInputLayout.GetAddressOf()
//		);
//
//		SetObjectName(outInputLayout, "Default Input Layout");
//
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create Input Layout!");
//			return false;
//		}
//
//		return true;
//	}
//
//	void RenderHardwareInterface::SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout) const
//	{
//		myContext->IASetInputLayout(aInputLayout.Get());
//	}
//
//	bool RenderHardwareInterface::LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
//	{
//		ComPtr<ID3D11ShaderReflection> shaderReflection;
//		HRESULT result = D3DReflect(
//			aShaderDataPtr,
//			aShaderDataSize,
//			IID_ID3D11ShaderReflection,
//			reinterpret_cast<void**>(shaderReflection.GetAddressOf())
//		);
//
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to load Shader!"); 
//			return false;
//		}
//
//		D3D11_SHADER_DESC shaderDesc = {};
//		shaderReflection->GetDesc(&shaderDesc);
//
//		D3D11_SHADER_VERSION_TYPE shaderVersion = static_cast<D3D11_SHADER_VERSION_TYPE>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));
//
//		switch (shaderVersion)
//		{
//		case D3D11_SHVER_VERTEX_SHADER:
//		{
//			outShader.myType = ShaderType::VertexShader;
//			ComPtr<ID3D11VertexShader> vsShader;
//			result = myDevice->CreateVertexShader(aShaderDataPtr, aShaderDataSize, nullptr, &vsShader);
//			outShader.myShader = vsShader;
//			break;
//		}
//		case D3D11_SHVER_PIXEL_SHADER:
//		{
//			outShader.myType = ShaderType::PixelShader;
//			ComPtr<ID3D11PixelShader> psShader;
//			result = myDevice->CreatePixelShader(aShaderDataPtr, aShaderDataSize, nullptr, &psShader);
//			outShader.myShader = psShader;
//			break;
//		}
//		case D3D11_SHVER_GEOMETRY_SHADER:
//		{
//			outShader.myType = ShaderType::GeometryShader;
//			ComPtr<ID3D11GeometryShader> gsShader;
//			result = myDevice->CreateGeometryShader(aShaderDataPtr, aShaderDataSize, nullptr, &gsShader);
//			outShader.myShader = gsShader;
//			break;
//		}
//		default:
//			break;
//		}
//
//		SetObjectName(outShader.myShader, aName);
//
//		return true;
//	}
//
//	void RenderHardwareInterface::Draw(unsigned aVertexCount)
//	{
//		myContext->Draw(aVertexCount, 0);
//	}
//
//	void RenderHardwareInterface::DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const
//	{
//		myContext->DrawIndexed(aIndexCount, aStartIndex, 0);
//	}
//
//	bool RenderHardwareInterface::ResizeBuffers(const CU::Vector2ui& aNewSize)
//	{
//		myBackBuffer = {};
//		myDepthBuffer = {};
//
//		const HRESULT result = mySwapChain->ResizeBuffers(0, aNewSize.x, aNewSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
//		if(FAILED(result))
//		{
//			LOG_ERROR("Failed to resize buffers. Did you forget to unbind something?");
//			return false;
//		}
//
//		return CreateBackBuffer(aNewSize) && CreateDepthBuffer(aNewSize);
//	}
//
//	void RenderHardwareInterface::ClearBackBuffer() const
//	{
//		myContext->ClearRenderTargetView(myBackBuffer->myRTV.Get(), myBackBuffer->myClearColor.data());
//		myContext->ClearDepthStencilView(myDepthBuffer->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	}
//
//	void RenderHardwareInterface::SetShaders(const Shader& aVxShader, const Shader& aPxShader) const
//	{
//		ComPtr<ID3D11VertexShader> vsShader;
//		aVxShader.myShader.As(&vsShader);
//		myContext->VSSetShader(vsShader.Get(), nullptr, 0);
//
//		ComPtr<ID3D11PixelShader> pxShader;
//		aPxShader.myShader.As(&pxShader);
//		myContext->PSSetShader(pxShader.Get(), nullptr, 0);
//	}
//
//	void RenderHardwareInterface::SetRenderTarget() const
//	{
//		myContext->OMSetRenderTargets(1, myBackBuffer->myRTV.GetAddressOf(), myDepthBuffer->myDSV.Get());
//		D3D11_VIEWPORT viewport = {};
//		viewport.TopLeftX = 0;
//		viewport.TopLeftY = 0;
//		viewport.Width = myBackBuffer->myViewPort.z;
//		viewport.Height = myBackBuffer->myViewPort.w;
//		viewport.MinDepth = 0;
//		viewport.MaxDepth = 1;
//		myContext->RSSetViewports(1, &viewport);
//	}
//
//	bool RenderHardwareInterface::CreateBackBuffer(const CU::Vector2ui& aBufferSize)
//	{
//		ComPtr<ID3D11Texture2D> backBufferTexture;
//		HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to retrieve back buffer!");
//			return false;
//		}
//
//		myBackBuffer = std::make_shared<Texture>();
//		result = myDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, myBackBuffer->myRTV.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create Render Target View!");
//			return false;
//		}
//
//		myBackBuffer->myViewPort = { 0, 0, static_cast<float>(aBufferSize.x), static_cast<float>(aBufferSize.y) };
//		return true;
//	}
//
//	bool RenderHardwareInterface::CreateDepthBuffer(const CU::Vector2ui& aBufferSize)
//	{
//		myDepthBuffer = std::make_shared<Texture>();
//
//		D3D11_TEXTURE2D_DESC depthDesc = {};
//		depthDesc.Width = static_cast<unsigned>(aBufferSize.x);
//		depthDesc.Height = static_cast<unsigned>(aBufferSize.y);
//		depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
//		depthDesc.Usage = D3D11_USAGE_DEFAULT;
//		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
//		depthDesc.CPUAccessFlags = 0;
//		depthDesc.MiscFlags = 0;
//		depthDesc.MipLevels = 1;
//		depthDesc.ArraySize = 1;
//		depthDesc.SampleDesc.Count = 1;
//		depthDesc.SampleDesc.Quality = 0;
//
//		ComPtr<ID3D11Texture2D> depthTexture;
//		HRESULT result = myDevice->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create depth buffer!");
//			return false;
//		}
//
//		SetObjectName(depthTexture, "DepthBuffer_T2D");
//
//		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
//		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//
//		result = myDevice->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myDepthBuffer->myDSV.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create depth stencil view!");
//			return false;
//		}
//
//		SetObjectName(myDepthBuffer->myDSV, "DepthBuffer_DSV");
//		return true;
//	}
//
//	bool RenderHardwareInterface::CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize)
//	{
//		D3D11_BUFFER_DESC vxBufferDesc = {};
//		vxBufferDesc.ByteWidth = static_cast<unsigned>(aNumVertices * aVertexSize);
//		vxBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//		vxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//
//		D3D11_SUBRESOURCE_DATA vxResource = {};
//		vxResource.pSysMem = aVertexDataPointer;
//
//		const HRESULT  result = myDevice->CreateBuffer(&vxBufferDesc, &vxResource, outVxBuffer.GetAddressOf());
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to create Vertex Buffer!");
//			return false;
//		}
//
//		SetObjectName(outVxBuffer, aName);
//
//		return true;
//	}
//
//	bool RenderHardwareInterface::UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const
//	{
//		if (!aBuffer.myBuffer)
//		{
//			LOG_ERROR("Failed to update constant buffer. Buffer {} is invalid.", aBuffer.myName);
//			return false;
//		}
//
//		if (aBufferDataSize > aBuffer.mySize)
//		{
//			LOG_ERROR("Failed to update constant buffer. Data too large.");
//			return false;
//		}
//
//		D3D11_MAPPED_SUBRESOURCE bufferData = {};
//		HRESULT result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
//		if (FAILED(result))
//		{
//			LOG_ERROR("Failed to map constant buffer {}.", aBuffer.myName);
//			return false;
//		}
//
//		memcpy_s(bufferData.pData, aBuffer.mySize, aBufferData, aBufferDataSize);
//		myContext->Unmap(aBuffer.myBuffer.Get(), 0);
//
//		return true;
//	}
//
//	void RenderHardwareInterface::SetObjectName(const ComPtr<ID3D11DeviceChild>& aObject, std::string_view aName) const
//	{
//		if (aObject)
//		{
//			aObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned>(sizeof(char) * aName.size()), aName.data());
//		}
//	}
//}