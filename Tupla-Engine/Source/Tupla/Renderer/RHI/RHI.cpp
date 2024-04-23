#include "tgpch.h"
#include "RHI.h"
#include <string>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <CommonUtilities/Strings.hpp>
#include <d3d11_1.h>
//#include <iostream>
//#include <fstream>
#include "Internal/DDSTextureLoader.h"
#include "Tupla/Renderer/WindowsWindow.h"
#include "../Primitives/Texture.h"
#include "../Primitives/Shader.h"
#include "../Primitives/Vertex.h"
#include "../Buffers/ConstantBuffer.h"
#include "../Buffers/ComputeBuffer.h"
#include "Tupla/Core/Application.h"
#include "PipelineStateObject.h"
//#include "../Debug/Instrumentor.h"

#define AUR_PROFILE_FUNCTION() 

namespace Tupla
{
	bool RHI::Init(const WindowsWindow& aWindow, Texture* outBackBuffer, Texture* outDepthBuffer, bool aEnableDeviceDebug)
	{
		AUR_PROFILE_FUNCTION();

		HRESULT result = E_FAIL;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = aWindow.GetWindowHandle();
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.BufferCount = 2;

		result = D3D11CreateDeviceAndSwapChain
		(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			aEnableDeviceDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mySwapChain,
			&myDevice,
			nullptr,
			&myContext
		);

		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create device and swap chain! %08X", result);
			return false;
		}

		// Verify compute shader is supported
		if (myDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
		{
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 };
			(void)myDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
			if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
			{
				myDevice->Release();
				LOG_CRITICAL("DirectCompute is not supported by this device.\n");
				return false;
			}
		}

		outBackBuffer->myName = "GraphicsEngine_BackBuffer";
		outBackBuffer->myBindFlags = D3D11_BIND_RENDER_TARGET;
		outBackBuffer->myUsageFlags = D3D11_USAGE_DEFAULT;
		outBackBuffer->myAccessFlags = 0;
		outBackBuffer->myType = TextureType::Texture2D;

		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &outBackBuffer->myTexture);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to retrieve backbuffer resource!");
			return false;
		}

		result = myDevice->CreateRenderTargetView(outBackBuffer->myTexture.Get(), nullptr, outBackBuffer->myRTV.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create RTV for backbuffer!");
			return false;
		}

		outBackBuffer->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(aWindow.GetWidth()), static_cast<float>(aWindow.GetHeight()), 0.0f, 1.0f });

		if (!CreateTexture(outDepthBuffer, "GraphicsEngine_DepthBuffer", static_cast<UINT>(aWindow.GetWidth()), static_cast<UINT>(aWindow.GetHeight()), DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL))
		{
			LOG_CRITICAL("Failed to create depth buffer!");
			return false;
		}


		myRasterizerStates[static_cast<unsigned>(RasterizerState::CullBack)] = nullptr;

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;
		myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[static_cast<unsigned>(RasterizerState::CullNone)].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[static_cast<unsigned>(RasterizerState::CullFront)].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[static_cast<unsigned>(RasterizerState::Wireframe)].GetAddressOf());


		myDepthStates[static_cast<unsigned>(DepthState::Default)] = nullptr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = false;
		myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStates[static_cast<unsigned>(DepthState::LessEqual)].GetAddressOf());

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;
		myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStates[static_cast<unsigned>(DepthState::ReadOnly)].GetAddressOf());

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		depthStencilDesc.StencilEnable = false;
		myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStates[static_cast<unsigned>(DepthState::GreaterEqual)].GetAddressOf());


		LOG_INFO("RHI has been intialized!");
		return true;
	}

	bool RHI::CreateTexture(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aFormat, D3D11_USAGE aUsageFlags, UINT aBindFlags, UINT aCpuAccessFlags)
	{
		if (aWidth < 1 || aHeight < 1 || aHeight > 25600 || aWidth > 25600)
		{
			LOG_WARN("I just tried to create a texture with dimensions X: {} Y: {} which is called {}", aWidth, aHeight, aName);
			aWidth = 4;
			aHeight = 4;
		}

		outTexture->myName = aName;
		outTexture->myBindFlags = aBindFlags;
		outTexture->myUsageFlags = aUsageFlags;
		outTexture->myAccessFlags = aCpuAccessFlags;
		outTexture->myWidth = aWidth;
		outTexture->myHeight = aHeight;
		outTexture->myType = TextureType::Texture2D;

		HRESULT result = S_FALSE;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = static_cast<unsigned>(aWidth);
		desc.Height = static_cast<unsigned>(aHeight);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = static_cast<DXGI_FORMAT>(aFormat);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = aUsageFlags;
		desc.BindFlags = aBindFlags;
		desc.CPUAccessFlags = aCpuAccessFlags;
		desc.MiscFlags = 0;


		result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(outTexture->myTexture.GetAddressOf()));
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create the requested texture!");
			return false;
		}

		if (aBindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
			depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			result = myDevice->CreateDepthStencilView(outTexture->myTexture.Get(), &depthDesc, outTexture->myDSV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a depth stencil view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc = {};
			depthSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
			depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			depthSRVDesc.Texture2D.MipLevels = desc.MipLevels;

			result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), (aBindFlags & D3D11_BIND_DEPTH_STENCIL) ? &depthSRVDesc : nullptr, outTexture->mySRV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a shader resource view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_RENDER_TARGET)
		{
			result = myDevice->CreateRenderTargetView(outTexture->myTexture.Get(), nullptr, outTexture->myRTV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a render target view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			if (aUsageFlags & D3D11_USAGE_DEFAULT && aCpuAccessFlags == 0)
			{
				result = myDevice->CreateUnorderedAccessView(outTexture->myTexture.Get(), nullptr, outTexture->myUAV.GetAddressOf());
				if (FAILED(result))
				{
					LOG_CRITICAL("Failed to create a unordered acces view!");
					return false;
				}
			}
		}

		//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.CPUAccessFlags = 0;


		D3D_SET_OBJECT_NAME_A(outTexture->myTexture, outTexture->myName.data());
		outTexture->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(aWidth), static_cast<float>(aHeight), 0.0f, 1.0f });
		return true;
	}

	bool RHI::CreateTexture3D(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aDepth, UINT aFormat, D3D11_USAGE aUsageFlags, UINT aBindFlags, UINT aCpuAccessFlags)
	{
		outTexture->myName = aName;
		outTexture->myBindFlags = aBindFlags;
		outTexture->myUsageFlags = aUsageFlags;
		outTexture->myAccessFlags = aCpuAccessFlags;
		outTexture->myWidth = aWidth;
		outTexture->myHeight = aHeight;
		outTexture->myDepth = aDepth;
		outTexture->myType = TextureType::Texture3D;

		HRESULT result = S_FALSE;

		D3D11_TEXTURE3D_DESC desc = {};
		desc.Width = static_cast<unsigned>(aWidth);
		desc.Height = static_cast<unsigned>(aHeight);
		desc.Depth = static_cast<unsigned>(aDepth);
		desc.MipLevels = 1;
		desc.Format = static_cast<DXGI_FORMAT>(aFormat);
		desc.Usage = aUsageFlags;
		desc.BindFlags = aBindFlags;
		desc.CPUAccessFlags = aCpuAccessFlags;
		desc.MiscFlags = 0;

		result = myDevice->CreateTexture3D(&desc, nullptr, reinterpret_cast<ID3D11Texture3D**>(outTexture->myTexture.GetAddressOf()));
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create the requested texture!");
			return false;
		}

		if (aBindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), nullptr, outTexture->mySRV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a shader resource view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_RENDER_TARGET)
		{
			result = myDevice->CreateRenderTargetView(outTexture->myTexture.Get(), nullptr, outTexture->myRTV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a render target view!");
				return false;
			}
		}


		if (aBindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			if ((aUsageFlags == D3D11_USAGE_DEFAULT) && aCpuAccessFlags == 0)
			{
				result = myDevice->CreateUnorderedAccessView(outTexture->myTexture.Get(), nullptr, outTexture->myUAV.GetAddressOf());
				if (FAILED(result))
				{
					LOG_CRITICAL("Failed to create a unordered acces view!");
					return false;
				}
			}
		}

		D3D_SET_OBJECT_NAME_A(outTexture->myTexture, outTexture->myName.data());
		outTexture->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(aWidth), static_cast<float>(aHeight), 0.0f, 1.0f });
		return true;
	}

	bool RHI::ResizeTexture(Texture* outTexture, UINT aWidth, UINT aHeight)
	{
		ASSERT(outTexture, "Can't resize a non-existant texture object!");

		return false;
	}

	bool RHI::CreateTextureCube(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aFormat, D3D11_USAGE aUsageFlags, UINT aBindFlags, UINT aCpuAccessFlags)
	{
		ASSERT(outTexture, "Initialize the Texture Object before calling this function!");
		outTexture->myName = aName;
		outTexture->myBindFlags = aBindFlags;
		outTexture->myUsageFlags = aUsageFlags;
		outTexture->myAccessFlags = aCpuAccessFlags;
		outTexture->myWidth = aWidth;
		outTexture->myHeight = aHeight;
		outTexture->myType = TextureType::TextureCube;

		HRESULT result = S_FALSE;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = static_cast<unsigned>(aWidth);
		desc.Height = static_cast<unsigned>(aHeight);
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		desc.Format = static_cast<DXGI_FORMAT>(aFormat);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = aUsageFlags;
		desc.BindFlags = aBindFlags;
		desc.CPUAccessFlags = aCpuAccessFlags;
		desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;


		result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(outTexture->myTexture.GetAddressOf()));
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create the requested texture!");
			return false;
		}

		if (aBindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
			depthDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT;
			depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthDesc.Texture2DArray.FirstArraySlice = 0;
			depthDesc.Texture2DArray.ArraySize = 6;

			result = myDevice->CreateDepthStencilView(outTexture->myTexture.Get(), &depthDesc, outTexture->myDSV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a depth stencil view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			// If we want a SRV and we're also a Depth Stencil we need
			// to specify these settings for the SRV to make sense.
			D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc = {};
			depthSRVDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT;
			depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			depthSRVDesc.TextureCube.MipLevels = desc.MipLevels;
			depthSRVDesc.TextureCube.MostDetailedMip = 0;

			result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(),
				(aBindFlags & D3D11_BIND_DEPTH_STENCIL) ? &depthSRVDesc : nullptr, outTexture->mySRV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a shader resource view!");
				return false;
			}
		}

		if (aBindFlags & D3D11_BIND_RENDER_TARGET)
		{
			result = myDevice->CreateRenderTargetView(outTexture->myTexture.Get(), nullptr, outTexture->myRTV.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create a render target view!");
				return false;
			}
		}
		D3D_SET_OBJECT_NAME_A(outTexture->myTexture, outTexture->myName.data());
		outTexture->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f,
			static_cast<float>(aWidth), static_cast<float>(aHeight),
			0.0f, 1.0f });

		return true;
	}

	/*bool RHI::LoadTexture(Texture* outTexture, const std::filesystem::path& aFilePath)
	{
		outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
		outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
		outTexture->myAccessFlags = 0;

		HRESULT result = DirectX::CreateDDSTextureFromFile
		(
			myDevice.Get(),
			CU::string_cast<std::wstring>(aFilePath.string()).c_str(),
			outTexture->myTexture.GetAddressOf(),
			outTexture->mySRV.GetAddressOf()
		);

		if (FAILED(result))
		{
			LOG_ERROR("Failed to load the requested texture: {}", aFilePath.string());
			return false;
		}

		result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), nullptr, outTexture->mySRV.GetAddressOf());
		if (FAILED(result))
		{
			LOG_ERROR("Failed to create a shader resource view: {}", aFilePath.string());
			return false;
		}


		bool dataSet = false;

		if (dataSet == false)
		{
			D3D11_TEXTURE3D_DESC desc{};
			ID3D11Texture3D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture3D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);
				outTexture->myWidth = desc.Width;
				outTexture->myHeight = desc.Height;
				outTexture->myDepth = desc.Depth;
				outTexture->myType = TextureType::Texture3D;
				dataSet = true;
			}
		}

		if (dataSet == false)
		{
			D3D11_TEXTURE2D_DESC desc{};
			ID3D11Texture2D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture2D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);

				outTexture->myWidth = desc.Width;
				outTexture->myHeight = desc.Height;

				if (desc.ArraySize == 6)
				{
					outTexture->myType = TextureType::TextureCube;
				}
				else
				{
					outTexture->myType = TextureType::Texture2D;
				}

				dataSet = true;
			}
		}

		if (dataSet == false)
		{
			D3D11_TEXTURE1D_DESC desc{};
			ID3D11Texture1D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture1D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);
				outTexture->myWidth = desc.Width;
				outTexture->myType = TextureType::Texture1D;
				dataSet = true;
			}
		}


		outTexture->myName = aFilePath.stem().string();
		D3D_SET_OBJECT_NAME_A(outTexture->myTexture, outTexture->myName.data());
		return true;
	}*/

	bool RHI::LoadTextureFromMemory(Texture* outTexture, const std::string& aName, const BYTE* aImageData, size_t aImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
	{
		outTexture->myName = aName;
		outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
		outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
		outTexture->myAccessFlags = 0;

		HRESULT result = DirectX::CreateDDSTextureFromMemory(
			myDevice.Get(),
			aImageData,
			aImageDataSize,
			(ID3D11Resource**)outTexture->myTexture.GetAddressOf(),
			outTexture->mySRV.GetAddressOf()
		);

		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to load the requested texture: " + aName);
			return false;
		}

		result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), aSRVDesc, outTexture->mySRV.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create a shader resource view!");
			return false;
		}


		bool dataSet = false;

		if (dataSet == false)
		{
			D3D11_TEXTURE3D_DESC desc{};
			ID3D11Texture3D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture3D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);
				outTexture->myWidth = desc.Width;
				outTexture->myHeight = desc.Height;
				outTexture->myDepth = desc.Depth;
				outTexture->myType = TextureType::Texture3D;
				dataSet = true;
			}
		}

		if (dataSet == false)
		{
			D3D11_TEXTURE2D_DESC desc{};
			ID3D11Texture2D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture2D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);

				outTexture->myWidth = desc.Width;
				outTexture->myHeight = desc.Height;

				if (desc.ArraySize == 6)
				{
					outTexture->myType = TextureType::TextureCube;
				}
				else
				{
					outTexture->myType = TextureType::Texture2D;
				}

				dataSet = true;
			}
		}

		if (dataSet == false)
		{
			D3D11_TEXTURE1D_DESC desc{};
			ID3D11Texture1D* tempPointer = nullptr;
			result = outTexture->myTexture->QueryInterface<ID3D11Texture1D>(&tempPointer);
			if (SUCCEEDED(result))
			{
				tempPointer->GetDesc(&desc);
				outTexture->myWidth = desc.Width;
				outTexture->myType = TextureType::Texture1D;
				dataSet = true;
			}
		}


		D3D_SET_OBJECT_NAME_A(outTexture->myTexture, outTexture->myName.data());
		return true;
	}

	void RHI::SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture* aTexture)
	{
		if ((aTexture->myBindFlags & D3D11_BIND_SHADER_RESOURCE) == false)
		{
			LOG_CRITICAL("Attempted to set a write only texture as shader resource!");
			return;
		}

		if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
		{
			myContext->VSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}

		if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
		{
			myContext->GSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}

		if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
		{
			myContext->PSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}

		if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
		{
			myContext->CSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}
	}

	void RHI::RemoveTextureResource(unsigned aPipelineStages, unsigned aSlot)
	{
		ID3D11ShaderResourceView* none = nullptr;

		if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
		{
			myContext->VSSetShaderResources(aSlot, 1, &none);
		}

		if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
		{
			myContext->GSSetShaderResources(aSlot, 1, &none);
		}

		if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
		{
			myContext->PSSetShaderResources(aSlot, 1, &none);
		}

		if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
		{
			myContext->CSSetShaderResources(aSlot, 1, &none);
		}
	}

	void RHI::SetTextureResources(UINT aPipelineStages, unsigned aStartSlot, const std::vector<Texture*>& aTextureList)
	{
		std::vector<ID3D11ShaderResourceView*> SRVs;
		SRVs.resize(aTextureList.size());
		for (size_t i = 0; i < aTextureList.size(); i++)
		{
			SRVs[i] = aTextureList[i]->mySRV.Get();
		}

		if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
		{
			myContext->VSSetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
		{
			myContext->GSGetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
		{
			myContext->PSSetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
		{
			myContext->CSSetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), SRVs.data());
		}
	}

	void RHI::RemoveTextureResources(UINT aPipelineStages, unsigned aStartSlot, unsigned aAmount)
	{
		std::vector<ID3D11ShaderResourceView*> SRVs;
		SRVs.resize(aAmount);

		if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
		{
			myContext->VSSetShaderResources(aStartSlot, aAmount, SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
		{
			myContext->GSGetShaderResources(aStartSlot, aAmount, SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
		{
			myContext->PSSetShaderResources(aStartSlot, aAmount, SRVs.data());
		}

		if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
		{
			myContext->CSSetShaderResources(aStartSlot, aAmount, SRVs.data());
		}
	}

	bool RHI::CreateDynamicVertexBuffer(ComPtr<ID3D11Buffer>& outVertexBuffer, const size_t aMaxNumberOfVertices, const size_t aVertexTypeSize)
	{
		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.ByteWidth = static_cast<UINT>(aMaxNumberOfVertices * aVertexTypeSize);
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

		const HRESULT result = myDevice->CreateBuffer(&vertexBufferDesc, nullptr, outVertexBuffer.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create dynamic vertex buffer!");
			return false;
		}

		return true;
	}

	bool RHI::CreateIndexBuffer(ComPtr<ID3D11Buffer>& outIndexBuffer, const std::vector<unsigned>& aIndexList)
	{
		D3D11_BUFFER_DESC indexBufferDesc{};
		indexBufferDesc.ByteWidth = static_cast<UINT>(aIndexList.size() * sizeof(unsigned int));
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubresourceData{};
		indexSubresourceData.pSysMem = &aIndexList[0];

		const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIndexBuffer.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create index buffer!");
			return false;
		}

		return true;
	}

	bool RHI::CreateDynamicIndexBuffer(ComPtr<ID3D11Buffer>& outIndexBuffer, const size_t aMaxNumberOfIndices)
	{
		D3D11_BUFFER_DESC indexBufferDesc{};
		indexBufferDesc.ByteWidth = static_cast<UINT>(aMaxNumberOfIndices * sizeof(unsigned int));
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

		const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, nullptr, outIndexBuffer.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create dynamic index buffer!");
			return false;
		}

		return true;
	}

	bool RHI::CreateInputLayout(ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayoutDesc, const BYTE* aShaderData, size_t aShaderDataSize)
	{
		if (!outInputLayout)
		{
			const HRESULT result = myDevice->CreateInputLayout
			(
				aInputLayoutDesc.data(),
				static_cast<UINT>(aInputLayoutDesc.size()),
				aShaderData,
				aShaderDataSize,
				outInputLayout.GetAddressOf()
			);

			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create input layout!");
				return false;
			}
		}

		return true;
	}

	bool RHI::CreateSamplerState(ComPtr<ID3D11SamplerState>& outSamplerState, const D3D11_SAMPLER_DESC& aDescription)
	{
		const HRESULT result = myDevice->CreateSamplerState(&aDescription, outSamplerState.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create sampler state!");
			return false;
		}

		return true;
	}

	void RHI::SetSamplerState(const ComPtr<ID3D11SamplerState>& aSamplerState, unsigned aSlot)
	{
		myContext->PSSetSamplers(aSlot, 1, aSamplerState.GetAddressOf());
	}

	/**bool RHI::LoadShaderFromFile(Shader* outShader, const std::filesystem::path& aFilePath)
	{
		ASSERT(outShader, "Please for the love of god, why is the outShader not initialized!?");

		if (std::filesystem::exists(aFilePath) == false)
		{
			LOG_ERROR("Couldn't find {} when trying to load a shader file!", aFilePath.string().c_str());
			return false;
		}

		std::ifstream shFile;
		shFile.open(aFilePath, std::ios::binary);
		const std::vector<uint8_t> shData = { std::istreambuf_iterator<char>(shFile), std::istreambuf_iterator<char>() };
		shFile.close();

		if (shFile.bad())
		{
			LOG_CRITICAL("Probably should look into why this file is bad: " + aFilePath.string());
			return false;
		}

		return LoadShaderFromMemory(outShader, aFilePath.stem().string(), shData.data(), shData.size());
	}*/

	bool RHI::LoadShaderFromMemory(Shader* outShader, const std::string& aName, const void* aShaderData, size_t aShaderDataSize)
	{
		outShader->myName = aName;

		// TODO: Make a shader reflect function
		{
			ComPtr<ID3D11ShaderReflection> Data;
			const HRESULT result = D3DReflect(aShaderData, aShaderDataSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(Data.GetAddressOf()));
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to reflect the provided shader!");
				return false;
			}

			ComPtr<ID3D11ShaderReflection> shaderReflection = Data;
			D3D11_SHADER_DESC shaderDesc;
			ZeroMemory(&shaderDesc, sizeof(D3D11_SHADER_DESC));
			shaderReflection->GetDesc(&shaderDesc);
			outShader->myType = static_cast<ShaderType>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));
		}


		switch (outShader->GetShaderType())
		{
		case ShaderType::VertexShader:
		{
			ComPtr<ID3D11VertexShader> vsShader;
			if (CreateVertexShader(vsShader, aShaderData, aShaderDataSize))
			{
				outShader->myShader = vsShader;
			}
			break;
		}
		case ShaderType::GeometryShader:
		{
			ComPtr<ID3D11GeometryShader> gmShader;
			if (CreateGeometryShader(gmShader, aShaderData, aShaderDataSize))
			{
				outShader->myShader = gmShader;
			}
			break;
		}
		case ShaderType::PixelShader:
		{
			ComPtr<ID3D11PixelShader> pxShader;
			if (CreatePixelShader(pxShader, aShaderData, aShaderDataSize))
			{
				outShader->myShader = pxShader;
			}
			break;
		}
		case ShaderType::ComputeShader:
		{
			ComPtr<ID3D11ComputeShader> cpShader;
			if (CreateComputeShader(cpShader, aShaderData, aShaderDataSize))
			{
				outShader->myShader = cpShader;
			}
			break;
		}
		default:
		{
			LOG_CRITICAL("Trying to load an unknown shader type!");
			break;
		}
		}

		D3D_SET_OBJECT_NAME_A(outShader->myShader, outShader->myName.data());

		return true;
	}

	bool RHI::CreateVertexShader(ComPtr<ID3D11VertexShader>& outShader, const void* aShaderData, size_t aShaderDataSize)
	{
		const HRESULT result = myDevice->CreateVertexShader(aShaderData, aShaderDataSize, nullptr, &outShader);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to load vertex shader from the specified file!");
			return false;
		}

		return true;
	}

	bool RHI::CreateGeometryShader(ComPtr<ID3D11GeometryShader>& outShader, const void* aShaderData, size_t aShaderDataSize)
	{
		const HRESULT result = myDevice->CreateGeometryShader(aShaderData, aShaderDataSize, nullptr, &outShader);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to load geometry shader from the specified file!");
			return false;
		}

		return true;
	}

	bool RHI::CreatePixelShader(ComPtr<ID3D11PixelShader>& outShader, const void* aShaderData, size_t aShaderDataSize)
	{
		const HRESULT result = myDevice->CreatePixelShader(aShaderData, aShaderDataSize, nullptr, &outShader);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to load pixel shader from the specified file!");
			return false;
		}

		return true;
	}

	bool RHI::CreateComputeShader(ComPtr<ID3D11ComputeShader>& outShader, const void* aShaderData, size_t aShaderDataSize)
	{
		const HRESULT result = myDevice->CreateComputeShader(aShaderData, aShaderDataSize, nullptr, &outShader);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to load compute shader from the specified file!");
			return false;
		}

		return true;
	}

	void RHI::SetShader(const Shader* aShader)
	{
		switch (aShader->GetShaderType())
		{
		case ShaderType::VertexShader:
			SetVertexShader(aShader);
			break;
		case ShaderType::GeometryShader:
			SetGeometryShader(aShader);
			break;
		case ShaderType::PixelShader:
			SetPixelShader(aShader);
			break;
		case ShaderType::ComputeShader:
			SetComputeShader(aShader);
			break;
		default:
			LOG_CRITICAL("Trying to set an unknown shader type!");
			break;
		}
	}

	void RHI::SetVertexShader(const Shader* aVertexShader)
	{
		if (aVertexShader == nullptr)
		{
			myContext->VSSetShader(nullptr, nullptr, 0);
			return;
		}

		if (aVertexShader->GetShaderType() != ShaderType::VertexShader)
		{
			LOG_CRITICAL("That's not a vertex shader!");
			return;
		}

		ComPtr<ID3D11VertexShader> vsShader;
		aVertexShader->GetShader().As(&vsShader);
		myContext->VSSetShader(vsShader.Get(), nullptr, 0);
	}

	void RHI::SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader)
	{
		myContext->VSSetShader(aVertexShader.Get(), nullptr, 0);
	}

	void RHI::SetGeometryShader(const Shader* aGeometryShader)
	{
		if (aGeometryShader == nullptr)
		{
			myContext->GSSetShader(nullptr, nullptr, 0);
			return;
		}

		if (aGeometryShader->GetShaderType() != ShaderType::GeometryShader)
		{
			LOG_CRITICAL("That's not a geometry shader! Carrot.");
			return;
		}

		ComPtr<ID3D11GeometryShader> gsShader;
		aGeometryShader->GetShader().As(&gsShader);
		myContext->GSSetShader(gsShader.Get(), nullptr, 0);
	}

	void RHI::SetGeometryShader(const ComPtr<ID3D11GeometryShader>& aGeometryShader)
	{
		myContext->GSSetShader(aGeometryShader.Get(), nullptr, 0);
	}

	void RHI::SetPixelShader(const Shader* aPixelShader)
	{
		if (aPixelShader == nullptr)
		{
			myContext->PSSetShader(nullptr, nullptr, 0);
			return;
		}

		if (aPixelShader->GetShaderType() != ShaderType::PixelShader)
		{
			LOG_CRITICAL("That's not a pixel shader! Potato.");
			return;
		}

		ComPtr<ID3D11PixelShader> psShader;
		aPixelShader->GetShader().As(&psShader);
		myContext->PSSetShader(psShader.Get(), nullptr, 0);
	}

	void RHI::SetPixelShader(const ComPtr<ID3D11PixelShader>& aPixelShader)
	{
		myContext->PSSetShader(aPixelShader.Get(), nullptr, 0);
	}

	void RHI::SetComputeShader(const Shader* aComputeShader)
	{
		if (aComputeShader == nullptr)
		{
			myContext->CSSetShader(nullptr, nullptr, 0);
			return;
		}

		if (aComputeShader->GetShaderType() != ShaderType::ComputeShader)
		{
			LOG_CRITICAL("That's not a compute shader! Onion.");
			return;
		}

		ComPtr<ID3D11ComputeShader> cpShader;
		aComputeShader->GetShader().As(&cpShader);
		myContext->CSSetShader(cpShader.Get(), nullptr, 0);
	}

	void RHI::SetComputeShader(const ComPtr<ID3D11ComputeShader>& aComputeShader)
	{
		myContext->CSSetShader(aComputeShader.Get(), nullptr, 0);
	}

	void RHI::SetPipelineStateObject(PipelineStateObject* aPSO)
	{
		const std::array<float, 4> blendFactor = { 0, 0, 0, 0 };
		constexpr unsigned samplerMask = 0xffffffff;
		myContext->OMSetBlendState(aPSO->BlendState.Get(), blendFactor.data(), samplerMask);
		myContext->RSSetState(aPSO->RasterizerState.Get());
		myContext->OMSetDepthStencilState(aPSO->DepthStencilState.Get(), 0);

		const std::shared_ptr<Shader> vShader = aPSO->VertexShader.lock();
		const std::shared_ptr<Shader> pShader = aPSO->PixelShader.lock();
		const std::shared_ptr<Shader> gShader = aPSO->GeometryShader.lock();

		ComPtr<ID3D11VertexShader> vsShader;
		if (vShader != nullptr)
		{
			vShader->GetShader().As(&vsShader);
			myContext->VSSetShader(vsShader.Get(), nullptr, 0);
		}
		else myContext->VSSetShader(nullptr, nullptr, 0);

		ComPtr<ID3D11PixelShader> psShader;
		if (pShader != nullptr)
		{
			pShader->GetShader().As(&psShader);
			myContext->PSSetShader(psShader.Get(), nullptr, 0);
		}
		else myContext->PSSetShader(nullptr, nullptr, 0);

		ComPtr<ID3D11GeometryShader> gsShader;
		if (gShader != nullptr)
		{
			gShader->GetShader().As(&gsShader);
			myContext->GSSetShader(gsShader.Get(), nullptr, 0);
		}
		else myContext->GSSetShader(nullptr, nullptr, 0);

		std::vector<ID3D11RenderTargetView*> rtvList;
		std::vector<D3D11_VIEWPORT> viewportList;
		rtvList.resize(aPSO->RenderTargets.size());
		viewportList.resize(aPSO->RenderTargets.size());

		CU::Vector4f clearColor = {};
		for (size_t r = 0; r < aPSO->RenderTargets.size(); r++)
		{
			if (aPSO->ClearRenderTargets)
			{
				myContext->ClearRenderTargetView(aPSO->RenderTargets[r]->myRTV.Get(), reinterpret_cast<float*>(&clearColor));
			}
			rtvList[r] = aPSO->RenderTargets[r]->myRTV.Get();
			viewportList[r] = { 0.0f, 0.0f, static_cast<float>(aPSO->RenderTargets[r]->Width()), static_cast<float>(aPSO->RenderTargets[r]->Height()), 0.0f, 1.0f };
		}

		if (aPSO->ClearDepthStencil)
		{
			if (aPSO->DepthStencil)
			{
				myContext->ClearDepthStencilView(aPSO->DepthStencil->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			}
		}
		if (aPSO->DepthStencil)
		{
			myContext->OMSetRenderTargets(static_cast<unsigned>(rtvList.size()), rtvList.data(), aPSO->DepthStencil->myDSV.Get());
		}
		else
		{
			myContext->OMSetRenderTargets(static_cast<unsigned>(rtvList.size()), rtvList.data(), nullptr);
		}
		myContext->RSSetViewports(static_cast<unsigned>(viewportList.size()), viewportList.data());

		myContext->IASetInputLayout(aPSO->InputLayout.Get());
		myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aPSO->Topology));

		if (!aPSO->PipelineTextureShaderResources.empty())
		{
			for (const PSOTextureShaderResource& resource : aPSO->PipelineTextureShaderResources)
			{
				if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
				{
					myContext->VSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_GEOMETRY_SHADER)
				{
					myContext->GSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
				{
					myContext->PSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					if (resource.writable)
					{
						const unsigned minusOne = -1;
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, resource.texture->myUAV.GetAddressOf(), &minusOne);
					}
					else
					{
						myContext->CSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
					}
				}
			}
		}

		if (!aPSO->PipelineStructuredShaderResources.empty())
		{
			for (const PSOStructuredShaderResource& resource : aPSO->PipelineStructuredShaderResources)
			{
				if (resource.writable)
				{
					if (resource.buffer->myUAV == nullptr)
					{
						LOG_CRITICAL("NAJ");
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						const UINT minusOne = -1; //Varf?r DX11!? VARF?R?
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, resource.buffer->myUAV.GetAddressOf(), &minusOne);
					}
				}
				else
				{
					if (resource.buffer->mySRV == nullptr)
					{
						LOG_CRITICAL("NAaJ");
					}

					if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
					{
						myContext->PSSetShaderResources(resource.slot, 1, resource.buffer->mySRV.GetAddressOf());
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						myContext->CSSetShaderResources(resource.slot, 1, resource.buffer->mySRV.GetAddressOf());
					}
				}
			}
		}
	}

	void RHI::SetComputePipelineStateObject(ComputePipelineStateObject* aPSO)
	{
		ComPtr<ID3D11ComputeShader> cpShader;
		if (aPSO->ComputeShader.lock() != nullptr)
		{
			aPSO->ComputeShader.lock()->GetShader().As(&cpShader);
			myContext->CSSetShader(cpShader.Get(), nullptr, 0);
		}
		else myContext->CSSetShader(nullptr, nullptr, 0);
		
		if (!aPSO->PipelineStructuredShaderResources.empty())
		{
			for (const PSOStructuredShaderResource& resource : aPSO->PipelineStructuredShaderResources)
			{
				if (resource.writable)
				{
					if (resource.buffer->myUAV == nullptr)
					{
						LOG_CRITICAL("NAJ");
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						const UINT minusOne = -1; //Varför DX11!? VARFÖR?
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, resource.buffer->myUAV.GetAddressOf(), &minusOne);
					}
				}
				else
				{
					if (resource.buffer->mySRV == nullptr)
					{
						LOG_CRITICAL("NAaJ");
					}

					if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
					{
						myContext->PSSetShaderResources(resource.slot, 1, resource.buffer->mySRV.GetAddressOf());
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						myContext->CSSetShaderResources(resource.slot, 1, resource.buffer->mySRV.GetAddressOf());
					}
				}
			}
		}

		if (!aPSO->PipelineTextureShaderResources.empty())
		{
			for (const PSOTextureShaderResource& resource : aPSO->PipelineTextureShaderResources)
			{
				if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
				{
					myContext->VSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_GEOMETRY_SHADER)
				{
					myContext->GSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
				{
					myContext->PSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
				}

				if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					if (resource.writable)
					{
						if (resource.texture->myUAV == nullptr)
						{
							LOG_CRITICAL("NAJ");
						}

						const unsigned minusOne = -1;
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, resource.texture->myUAV.GetAddressOf(), &minusOne);
					}
					else
					{
						myContext->CSSetShaderResources(resource.slot, 1, resource.texture->mySRV.GetAddressOf());
					}
				}
			}
		}
	}

	void RHI::RemovePipelineStateObject(PipelineStateObject* aPSO)
	{
		if (!aPSO->RenderTargets.empty())
		{
			std::vector<ID3D11RenderTargetView*> nullRTVs;
			nullRTVs.resize(aPSO->RenderTargets.size());
			myContext->OMSetRenderTargets(static_cast<unsigned>(nullRTVs.size()), nullRTVs.data(), nullptr);
		}

		else
		{
			if (aPSO->DepthStencil)
			{
				myContext->OMSetRenderTargets(0, nullptr, nullptr);
			}
		}

		if (!aPSO->PipelineTextureShaderResources.empty())
		{
			std::vector<ID3D11ShaderResourceView*> nullSRV;
			nullSRV.resize(1);
			for (const PSOTextureShaderResource& resource : aPSO->PipelineTextureShaderResources)
			{
				if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
				{
					myContext->VSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_GEOMETRY_SHADER)
				{
					myContext->GSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
				{
					myContext->PSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					myContext->CSSetShaderResources(resource.slot, 1, nullSRV.data());
				}
			}
		}

		if (!aPSO->PipelineStructuredShaderResources.empty())
		{
			std::vector<ID3D11ShaderResourceView*> nullSRV;
			nullSRV.resize(1);

			std::vector<ID3D11UnorderedAccessView*> nullVs;
			nullVs.resize(1);

			for (const PSOStructuredShaderResource& resource : aPSO->PipelineStructuredShaderResources)
			{
				if (resource.writable)
				{
					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						const UINT minusOne = -1; //Varf?r DX11!? VARF?R?
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, nullVs.data(), &minusOne);
					}
				}
				else
				{
					if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->PSSetShaderResources(resource.slot, 1, nullVs.data());
					}

					if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->PSSetShaderResources(resource.slot, 1, nullVs.data());
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->CSSetShaderResources(resource.slot, 1, nullVs.data());
					}
				}
			}
		}
	}

	void RHI::RemoveComputePipelineStateObject(ComputePipelineStateObject* aPSO)
	{
		myContext->CSSetShader(nullptr, nullptr, 0);
		if (!aPSO->PipelineTextureShaderResources.empty())
		{
			std::vector<ID3D11ShaderResourceView*> nullSRV;
			nullSRV.resize(1);
			for (const PSOTextureShaderResource& resource : aPSO->PipelineTextureShaderResources)
			{
				if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
				{
					myContext->VSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_GEOMETRY_SHADER)
				{
					myContext->GSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
				{
					myContext->PSSetShaderResources(resource.slot, 1, nullSRV.data());
				}

				if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					myContext->CSSetShaderResources(resource.slot, 1, nullSRV.data());
				}
			}
		}

		if (!aPSO->PipelineStructuredShaderResources.empty())
		{
			std::vector<ID3D11ShaderResourceView*> nullSRV;
			nullSRV.resize(1);

			std::vector<ID3D11UnorderedAccessView*> nullVs;
			nullVs.resize(1);

			for (const PSOStructuredShaderResource& resource : aPSO->PipelineStructuredShaderResources)
			{
				if (resource.writable)
				{
					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						const UINT minusOne = -1; //Varför DX11!? VARFÖR?
						myContext->CSSetUnorderedAccessViews(resource.slot, 1, nullVs.data(), &minusOne);
					}
				}
				else
				{
					if (resource.stage & PIPELINE_STAGE_VERTEX_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->PSSetShaderResources(resource.slot, 1, nullVs.data());
					}

					if (resource.stage & PIPELINE_STAGE_PIXEL_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->PSSetShaderResources(resource.slot, 1, nullVs.data());
					}

					if (resource.stage & PIPELINE_STAGE_COMPUTE_SHADER)
					{
						std::vector<ID3D11ShaderResourceView*> nullVs;
						nullVs.resize(1);
						myContext->CSSetShaderResources(resource.slot, 1, nullVs.data());
					}
				}
			}
		}
	}

	bool RHI::CreateBlendState(ComPtr<ID3D11BlendState>& outBlendState, const D3D11_BLEND_DESC& aDescription)
	{
		const HRESULT result = myDevice->CreateBlendState(&aDescription, outBlendState.GetAddressOf());

		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create blend state! Chicken broth.");
			return false;
		}

		return true;
	}

	void RHI::SetBlendState(const ComPtr<ID3D11BlendState>& aBlendState, const std::array<float, 4>& aBlendFactor, unsigned aSamplerMask)
	{
		myContext->OMSetBlendState(aBlendState.Get(), aBlendFactor.data(), aSamplerMask);
	}

	void RHI::GetRasterizerState(ComPtr<ID3D11RasterizerState>& outRasterizerState, const RasterizerState aState)
	{
		outRasterizerState = myRasterizerStates[static_cast<unsigned>(aState)];
	}

	void RHI::SetRasterizerState(const RasterizerState aState)
	{
		myContext->RSSetState(myRasterizerStates[static_cast<unsigned>(aState)].Get());
	}

	void RHI::GetDepthState(ComPtr<ID3D11DepthStencilState>& outDepthStencilState, const DepthState aState)
	{
		outDepthStencilState = myDepthStates[static_cast<unsigned>(aState)];
	}

	void RHI::SetDepthState(const DepthState aState)
	{
		myContext->OMSetDepthStencilState(myDepthStates[static_cast<unsigned>(aState)].Get(), 0);
	}

	void RHI::SetRenderTarget(const Texture* aTarget, const Texture* aDepthStencil)
	{
		myContext->OMSetRenderTargets(aTarget ? 1 : 0, aTarget ? aTarget->myRTV.GetAddressOf() : nullptr, aDepthStencil ? aDepthStencil->myDSV.Get() : nullptr);

		if (aTarget)
		{
			myContext->RSSetViewports(1, &aTarget->myViewport);
		}
		else if (aDepthStencil)
		{
			myContext->RSSetViewports(1, &aDepthStencil->myViewport);
		}
	}

	void RHI::SetRenderTargets(const std::vector<Texture*>& aTargetList, const Texture* aDepthStencil)
	{
		std::vector<ID3D11RenderTargetView*> myRTVs;
		myRTVs.reserve(aTargetList.size());

		for (const auto i : aTargetList)
		{
			if ((i->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
			{
				LOG_CRITICAL("Attempted to set an invalid or read only texture as Render Target! Water.");
			}

			myRTVs.push_back(i->myRTV.Get());
		}

		const UINT numRenderTargets = static_cast<UINT>(aTargetList.size());

		myContext->OMSetRenderTargets(numRenderTargets, myRTVs.data(), aDepthStencil->myDSV.Get());
		if (aTargetList[0])
		{
			myContext->RSSetViewports(1, &aTargetList[0]->myViewport);
		}
		else if (aDepthStencil)
		{
			myContext->RSSetViewports(1, &aDepthStencil->myViewport);
		}
	}

	void RHI::RemoveRenderTargets()
	{
		myContext->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void RHI::ClearRenderTarget(const Texture* aTexture, std::array<float, 4> aClearColor)
	{
		if ((aTexture->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
		{
			LOG_ERROR("Attempted to clear a read-only texture! Salt and Pepper?");
			return;
		}

		myContext->ClearRenderTargetView(aTexture->myRTV.Get(), aClearColor.data());
	}

	void RHI::ClearRenderTargets(const std::vector<Texture*>& aTargetList, std::array<float, 4> aClearColor)
	{
		for (const auto i : aTargetList)
		{
			if ((i->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
			{
				LOG_ERROR("Attempted to clear a read-only texture! Spices after personal taste.");
				continue;
			}

			myContext->ClearRenderTargetView(i->myRTV.Get(), aClearColor.data());
		}
	}

	void RHI::ClearDepthStencil(const Texture* aTexture)
	{
		if ((aTexture->myBindFlags & D3D11_BIND_DEPTH_STENCIL) == false)
		{
			LOG_CRITICAL("Attempted to clear depth on a non-depth texture!");
		}

		myContext->ClearDepthStencilView(aTexture->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	bool RHI::CreateComputeBuffer(ComputeBuffer* aBuffer, StructuredBufferType aBufferType, UINT aSize, UINT aNum, const std::string& aName, bool aShouldCreateWithData)
	{
		ASSERT(aSize <= 65536, "Exceeded max CBuffer size per buffer in bytes!");

		D3D11_BUFFER_DESC desc = { 0 };

		aBuffer->myDataSize = aNum * aSize;

		desc.ByteWidth = aNum * aSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = static_cast<UINT>(aSize);

		switch (aBufferType)
		{
		case StructuredBufferType::Default:
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		}
		break;
		case StructuredBufferType::Dynamic:
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		break;
		case StructuredBufferType::Immutable:
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}
		break;
		case StructuredBufferType::Staging:
		{
			desc.BindFlags = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		}
		break;
		default:
			break;
		}

		HRESULT result;
		/*if (aShouldCreateWithData)*/
		{
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = aBuffer->myDataPtr;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			result = myDevice->CreateBuffer(&desc, &data, aBuffer->buffer.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create compute buffer!");
				return false;
			}
		}
		/*else
		{
			result = myDevice->CreateBuffer(&desc, nullptr, aBuffer->myBuffer.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create compute buffer!");
				return false;
			}
		}*/


		//An SRV is always needed if we are going to use the buffer as a shader resource
		if (aBufferType != StructuredBufferType::Staging)
		{
			result = myDevice->CreateShaderResourceView(aBuffer->buffer.Get(), nullptr, &aBuffer->mySRV);
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create shader resource view!");
				return false;
			}
		}


		//UAV is only needed for GPU to write to the resource/buffer
		if (aBufferType == StructuredBufferType::Default)
		{
			result = myDevice->CreateUnorderedAccessView(aBuffer->buffer.Get(), nullptr, &aBuffer->myUAV);
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create unordered access view!");
				return false;
			}
		}

		D3D_SET_OBJECT_NAME_A(aBuffer->buffer.Get(), aName.c_str());
		return true;
	}

	void RHI::UpdateComputeBufferData(const ComputeBuffer& aBuffer)
	{
		if (aBuffer.myStructuredType != (unsigned)StructuredBufferType::Staging || aBuffer.myStructuredType == (unsigned)StructuredBufferType::Default)
		{
			return;
		}

		HRESULT result = S_FALSE;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(aBuffer.buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to map compute buffer when trying to update data!");
		}

		memcpy_s(bufferData.pData, aBuffer.myDataSize, aBuffer.myDataPtr, aBuffer.myDataSize);
		myContext->Unmap(aBuffer.buffer.Get(), 0);
	}

	void RHI::SetComputeBuffer(UINT aPipelineStages, unsigned aSlot, ComputeBuffer* aBuffer, bool aIsWritable)
	{
		if (aBuffer == nullptr)
		{
			if (aIsWritable)
			{
				if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					std::vector<ID3D11UnorderedAccessView*> nullVs;
					nullVs.resize(1);
					const UINT minusOne = -1; //Varf?r DX11!? VARF?R?
					myContext->CSSetUnorderedAccessViews(aSlot, 1, nullVs.data(), &minusOne);
				}
			}
			else
			{
				if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
				{
					std::vector<ID3D11ShaderResourceView*> nullVs;
					nullVs.resize(1);
					myContext->PSSetShaderResources(aSlot, 1, nullVs.data());
				}

				if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
				{
					std::vector<ID3D11ShaderResourceView*> nullVs;
					nullVs.resize(1);
					myContext->CSSetShaderResources(aSlot, 1, nullVs.data());
				}
			}

			return;
		}

		if (aIsWritable)
		{
			if (aBuffer->myUAV == nullptr)
			{
				LOG_CRITICAL("NAJ");
			}

			if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
			{
				const UINT minusOne = -1; //Varf?r DX11!? VARF?R?
				myContext->CSSetUnorderedAccessViews(aSlot, 1, aBuffer->myUAV.GetAddressOf(), &minusOne);
			}
		}
		else
		{
			if (aBuffer->mySRV == nullptr)
			{
				LOG_CRITICAL("NAaJ");
			}

			if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
			{
				myContext->PSSetShaderResources(aSlot, 1, aBuffer->mySRV.GetAddressOf());
			}

			if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
			{
				myContext->CSSetShaderResources(aSlot, 1, aBuffer->mySRV.GetAddressOf());
			}
		}
	}

	bool RHI::CreateAppendConsumeBuffer(ComputeBuffer* outBuffer, UINT aStructSize, UINT aCount, const std::string& aName, bool aShouldCreateWithData)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = aCount * aStructSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = aStructSize;

		// Select the appropriate usage and CPU access flags based on the passed in flags
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		HRESULT result;
		D3D11_SUBRESOURCE_DATA pData{};
		if (aShouldCreateWithData)
		{
			pData.pSysMem = outBuffer->myDataPtr;
			pData.SysMemPitch = 0;
			pData.SysMemSlicePitch = 0;
			result = myDevice->CreateBuffer(&desc, &pData, &outBuffer->buffer);
		}
		else
		{
			result = myDevice->CreateBuffer(&desc, nullptr, &outBuffer->buffer);
		}

		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create Append/Consume buffer! Name: {}", aName);
		}


		result = myDevice->CreateShaderResourceView(outBuffer->buffer.Get(), nullptr, &outBuffer->mySRV);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create shader resource view!");
			return false;
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_UNKNOWN;
		viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		result = myDevice->CreateUnorderedAccessView(outBuffer->buffer.Get(), nullptr, &outBuffer->myUAV);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create unordered access view!");
			return false;
		}
		return true;
	}

	bool RHI::CreateConstantBuffer(ComPtr<ID3D11Buffer>& outCBuffer, size_t aSize, const std::string& aName, UINT aBindFlag)
	{
		ASSERT(aSize <= 65536, "Exceeded max CBuffer size per buffer in bytes!");

		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = aBindFlag;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = static_cast<UINT>(aSize);

		const HRESULT result = myDevice->CreateBuffer(&bufferDescription, nullptr, outCBuffer.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create constant buffer!");
			return false;
		}

		D3D_SET_OBJECT_NAME_A(outCBuffer.Get(), aName.c_str());
		return true;
	}

	void RHI::UpdateConstantBufferData(const ConstantBuffer& aBuffer)
	{
		HRESULT result = S_FALSE;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to map constant buffer when trying to update data!");
		}

		memcpy(bufferData.pData, aBuffer.myDataPtr, aBuffer.mySize);
		myContext->Unmap(aBuffer.myBuffer.Get(), 0);
	}

	void RHI::SetConstantBuffer(UINT aPipelineStages, unsigned aSlot, const ConstantBuffer& aBuffer)
	{
		if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
		{
			myContext->VSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
		}

		if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
		{
			myContext->GSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
		}

		if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
		{
			myContext->PSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
		}

		//Constant buffers probably maybe might perhaps exist on compute shader? :)
		if (aPipelineStages & PIPELINE_STAGE_COMPUTE_SHADER)
		{
			myContext->CSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
		}
	}

	void RHI::SetVertexAndIndexBuffers(const std::vector<ComPtr<ID3D11Buffer>>& aVertexBuffers, const ComPtr<ID3D11Buffer>& aIndexBuffer, const std::vector<unsigned>& aVertexStrides)
	{
		std::vector<ID3D11Buffer*> buffers;
		buffers.reserve(aVertexBuffers.size());
		std::vector<unsigned> vxOffset;
		vxOffset.reserve(aVertexBuffers.size());

		for (size_t i = 0; i < aVertexBuffers.size(); i++)
		{
			buffers.push_back(aVertexBuffers[i].Get());
			vxOffset.push_back(0);
		}
		myContext->IASetVertexBuffers(0, static_cast<unsigned>(aVertexBuffers.size()), buffers.data(), aVertexStrides.data(), vxOffset.data());

		DXGI_FORMAT indexBufferFormat;
		aIndexBuffer == nullptr ? indexBufferFormat = DXGI_FORMAT_UNKNOWN : indexBufferFormat = DXGI_FORMAT_R32_UINT;
		myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
	}

	void RHI::SetVertexAndIndexBuffers(const ComPtr<ID3D11Buffer>& aVertexBuffer, const ComPtr<ID3D11Buffer>& aIndexBuffer, unsigned aVertexStrides)
	{
		constexpr unsigned vxOffset = 0;
		myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &aVertexStrides, &vxOffset);

		DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
		if (aIndexBuffer == nullptr)
		{
			indexBufferFormat = DXGI_FORMAT_UNKNOWN;
		}
		myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
	}

	void RHI::SetInputLayout(const ComPtr<ID3D11InputLayout>& aInputLayout)
	{
		myContext->IASetInputLayout(aInputLayout.Get());
	}

	void RHI::SetTopology(unsigned aTopology)
	{
		myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
	}

	void RHI::Dispatch(unsigned aX, unsigned aY, unsigned aZ)
	{
		myContext->Dispatch(aX, aY, aZ);
	}

	void RHI::Draw(unsigned aCount)
	{
		myContext->Draw(aCount, 0);
	}

	void RHI::DrawIndexed(unsigned aCount)
	{
		myContext->DrawIndexed(aCount, 0, 0);
	}

	void RHI::DrawIndexedInstanced(unsigned aCount, unsigned aInstanceCount)
	{
		myContext->DrawIndexedInstanced(aCount, aInstanceCount, 0, 0, 0);
	}

	void RHI::CopyRegion(const Texture* aSource, const Texture* aDestination, unsigned X, unsigned Y, unsigned Width, unsigned Height)
	{
		D3D11_BOX pickBox = {};
		pickBox.left = X;
		pickBox.right = X + Width;
		pickBox.top = Y;
		pickBox.bottom = Y + Height;
		pickBox.front = 0;
		pickBox.back = 1;

		ID3D11Resource* objIdResource = nullptr;
		aSource->myRTV.Get()->GetResource(&objIdResource);
		myContext->CopySubresourceRegion(aDestination->myTexture.Get(), 0, 0, 0, 0, objIdResource, 0, &pickBox);
	}

	CU::Vector4b RHI::GetSinglePixelValue(const Texture* aSource)
	{
		CU::Vector4b bytes = { 0, 0, 0, 0 };

		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		myContext->Map(aSource->myTexture.Get(), 0, D3D11_MAP_READ, 0, &resource);
		if (resource.pData != nullptr)
		{
			memcpy(&bytes, resource.pData, sizeof(bytes));
		}
		myContext->Unmap(aSource->myTexture.Get(), 0);

		return bytes;
	}

	CU::Vector4f RHI::GetSinglePixelValueFloat(const Texture* aSource)
	{
		CU::Vector4f bytes = { 0, 0, 0, 0 };

		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		myContext->Map(aSource->myTexture.Get(), 0, D3D11_MAP_READ, 0, &resource);
		if (resource.pData != nullptr)
		{
			memcpy(&bytes, resource.pData, sizeof(bytes));
		}
		myContext->Unmap(aSource->myTexture.Get(), 0);

		return bytes;
	}

	const bool RHI::ResizeDevice(Texture* outBackBuffer, Texture* outDepthBuffer)
	{
		myContext->OMSetRenderTargets(0, 0, 0);
		HRESULT result = E_FAIL;

		result = mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to resize buffers when trying to resize device window!");
			return false;
		}

		// Create the BackBuffer object that our Graphics Engine will use.
		// This is a special case that can't use our own CreateTexture function because
		// DirectX has already created its own representation via D3D11CreateDeviceAndSwapChain
		// and we just need a place to contain it for our own purposes.
		//outBackBuffer = std::make_shared<Texture>();
		outBackBuffer->myBindFlags = D3D11_BIND_RENDER_TARGET;
		outBackBuffer->myUsageFlags = D3D11_USAGE_DEFAULT;
		outBackBuffer->myAccessFlags = 0;

		// Retrieve back buffer texture that was created when we called CreateDeviceAndSwapChain.
		ComPtr<ID3D11Texture2D> backBufferTexture;
		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to retrieve backbuffer resource when trying to resize device window!");
			return false;
		}
		outBackBuffer->myTexture = backBufferTexture;
		// Create a View for the BackBuffer Texture that allows us to draw on the texture.
		result = myDevice->CreateRenderTargetView(outBackBuffer->myTexture.Get(), nullptr, outBackBuffer->myRTV.GetAddressOf());
		if (FAILED(result))
		{
			LOG_CRITICAL("Failed to create the RTV for backbuffer!");
			return false;
		}

		RECT clientRect;
		ZeroMemory(&clientRect, sizeof(RECT));
		GetClientRect(Application::Get().GetRenderer()->GetWindow()->GetWindowHandle(), &clientRect);
		const unsigned windowWidth = clientRect.right - clientRect.left;
		const unsigned windowHeight = clientRect.bottom - clientRect.top;

		outBackBuffer->myName = "GraphicsEngine_BackBuffer";
		outBackBuffer->myWidth = windowWidth;
		outBackBuffer->myHeight = windowHeight;
		outBackBuffer->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, 1.0f });

		D3D11_TEXTURE2D_DESC desc = {};
		backBufferTexture->GetDesc(&desc);

		// For the Scene Depth we can just use our own function to create a depth texture.
		if (CreateTexture(outDepthBuffer, "GraphicsEngine_DepthBuffer", windowWidth, windowHeight,
			DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL) == false)
		{
			LOG_CRITICAL("Failed to create depth buffer!");
			return false;
		}

		outDepthBuffer->myName = "GraphicsEngine_DepthBuffer";
		outDepthBuffer->myWidth = windowWidth;
		outDepthBuffer->myHeight = windowHeight;

		return true;
	}

	void RHI::Present(unsigned aSyncInterval)
	{
		mySwapChain->Present(aSyncInterval, DXGI_PRESENT_ALLOW_TEARING);
		//mySwapChain->Present(1, 0);
	}
}
