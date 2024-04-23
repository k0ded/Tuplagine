#pragma once
#include <string>
#include <array>
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <memory>
#include <filesystem>
#include "CommonUtilities/Math/Vector/Vector4.hpp"
#include "Tupla/Renderer/Buffers/ComputeBuffer.h"
#include "Tupla/Core/Log.h"

using namespace Microsoft::WRL;

namespace Tupla
{
	class ConstantBuffer;
	class ComputeBuffer;

	class WindowsWindow;
	class Texture;
	class Shader;
	struct Vertex;
	struct PipelineStateObject;
	struct ComputePipelineStateObject;

	enum class RasterizerState
	{
		CullBack,
		CullNone,
		CullFront,
		Wireframe,
		COUNT
	};

	enum class DepthState
	{
		Default,
		LessEqual,
		ReadOnly,
		GreaterEqual,
		COUNT
	};

	/// <summary>
	/// Default means GPU only.
	/// Dynamic GPU readable, CPU Writeable.
	/// Immutable is static, only read on GPU.
	/// Staging full access from both but not at the same time(!)
	/// </summary>
	enum class StructuredBufferType : unsigned
	{
		Default,
		Dynamic,
		Immutable,
		Staging
	};

	class RHI
	{
	public:
		RHI() = delete;
		~RHI() = delete;

		static bool Init(const WindowsWindow& aWindow, Texture* outBackBuffer, Texture* outDepthBuffer, bool aEnableDeviceDebug = false);

		static bool CreateTexture(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aFormat, D3D11_USAGE aUsageFlags = D3D11_USAGE_DEFAULT, UINT aBindFlags = D3D11_BIND_SHADER_RESOURCE, UINT aCpuAccessFlags = 0);
		static bool CreateTexture3D(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aDepth, UINT aFormat, D3D11_USAGE aUsageFlags = D3D11_USAGE_DEFAULT, UINT aBindFlags = D3D11_BIND_SHADER_RESOURCE, UINT aCpuAccessFlags = 0);
		static bool ResizeTexture(Texture* outTexture, UINT aWidth, UINT aHeight); //TODO: Be sad because this doesn't work yet
		static bool CreateTextureCube(Texture* outTexture, const std::string& aName, UINT aWidth, UINT aHeight, UINT aFormat, D3D11_USAGE aUsageFlags = D3D11_USAGE_DEFAULT, UINT aBindFlags = D3D11_BIND_SHADER_RESOURCE,UINT aCpuAccessFlags = 0);
		
		//static bool LoadTexture(Texture* outTexture, const std::filesystem::path& aFilePath);
		static bool LoadTextureFromMemory(Texture* outTexture, const std::string& aName, const BYTE* aImageData, size_t aImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);
		static void SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture* aTexture);
		static void RemoveTextureResource(unsigned aPipelineStages, unsigned aSlot);
		static void SetTextureResources(UINT aPipelineStages, unsigned aStartSlot, const std::vector<Texture*>& aTextureList);
		static void RemoveTextureResources(UINT aPipelineStages, unsigned aStartSlot, unsigned aAmount);

		template <typename T>
		static bool CreateVertexBuffer(ComPtr<ID3D11Buffer>& outVertexBuffer, const std::vector<T>& aVertexList)
		{
			const size_t vxSize = sizeof(T);
			const size_t numVx = aVertexList.size();

			D3D11_BUFFER_DESC vertexBufferDesc{};
			vertexBufferDesc.ByteWidth = static_cast<UINT>(numVx * vxSize);
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
			vertexSubresourceData.pSysMem = reinterpret_cast<const uint8_t*>(aVertexList.data());

			const HRESULT result = myDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, outVertexBuffer.GetAddressOf());
			if (FAILED(result))
			{
				LOG_CRITICAL("Failed to create vertex buffer!");
				return false;
			}

			return true;
		}

		static bool CreateDynamicVertexBuffer(ComPtr<ID3D11Buffer>& outVertexBuffer, const size_t aMaxNumberOfVertices, const size_t aVertexTypeSize);

		static bool CreateIndexBuffer(ComPtr<ID3D11Buffer>& outIndexBuffer, const std::vector<unsigned>& aIndexList);
		static bool CreateDynamicIndexBuffer(ComPtr<ID3D11Buffer>& outIndexBuffer, const size_t aMaxNumberOfIndices);

		static bool CreateInputLayout(ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayoutDesc, const BYTE* aShaderData, size_t aShaderDataSize);

		static bool CreateSamplerState(ComPtr<ID3D11SamplerState>& outSamplerState, const D3D11_SAMPLER_DESC& aDescription);
		static void SetSamplerState(const ComPtr<ID3D11SamplerState>& aSamplerState, unsigned aSlot);

		//static bool LoadShaderFromFile(Shader* outShader, const std::filesystem::path& aFilePath);
		static bool LoadShaderFromMemory(Shader* outShader, const std::string& aName, const void* aShaderData, size_t aShaderDataSize);
		static bool CreateVertexShader(ComPtr<ID3D11VertexShader>& outShader, const void* aShaderData, size_t aShaderDataSize);
		static bool CreateGeometryShader(ComPtr<ID3D11GeometryShader>& outShader, const void* aShaderData, size_t aShaderDataSize);
		static bool CreatePixelShader(ComPtr<ID3D11PixelShader>& outShader, const void* aShaderData, size_t aShaderDataSize);
		static bool CreateComputeShader(ComPtr<ID3D11ComputeShader>& outShader, const void* aShaderData, size_t aShaderDataSize);


		static void SetShader(const Shader* aShader);

		static void SetVertexShader(const Shader* aVertexShader);
		static void SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader);

		static void SetGeometryShader(const Shader* aGeometryShader);
		static void SetGeometryShader(const ComPtr<ID3D11GeometryShader>& aGeometryShader);

		static void SetPixelShader(const Shader* aPixelShader);
		static void SetPixelShader(const ComPtr<ID3D11PixelShader>& aPixelShader);

		static void SetComputeShader(const Shader* aComputeShader);
		static void SetComputeShader(const ComPtr<ID3D11ComputeShader>& aComputeShader);

		static void SetPipelineStateObject(PipelineStateObject* aPSO);
		static void SetComputePipelineStateObject(ComputePipelineStateObject* aPSO);

		static void RemovePipelineStateObject(PipelineStateObject* aPSO);
		static void RemoveComputePipelineStateObject(ComputePipelineStateObject* aPSO);

		static bool CreateBlendState(ComPtr<ID3D11BlendState>& outBlendState, const D3D11_BLEND_DESC& aDescription);

		static void SetBlendState(const ComPtr<ID3D11BlendState>& aBlendState, const std::array<float, 4>& aBlendFactor = { 0, 0, 0, 0 }, unsigned aSamplerMask = 0xffffffff);


		static void GetRasterizerState(ComPtr<ID3D11RasterizerState>& outRasterizerState, const RasterizerState aState);
		static void SetRasterizerState(const RasterizerState aState);

		static void GetDepthState(ComPtr<ID3D11DepthStencilState>& outDepthStencilState, const DepthState aState);
		static void SetDepthState(const DepthState aState);

		static void SetRenderTarget(const Texture* aTarget, const Texture* aDepthStencil);
		static void SetRenderTargets(const std::vector<Texture*>& aTargetList, const Texture* aDepthStencil);
		static void RemoveRenderTargets();


		static void ClearRenderTarget(const Texture* aTexture, std::array<float, 4> aClearColor = { 0.0f, 0.0f, 0.0f, 0.0f });
		static void ClearRenderTargets(const std::vector<Texture*>& aTargetList, std::array<float, 4> aClearColor = { 0.0f, 0.0f, 0.0f, 0.0f });
		static void ClearDepthStencil(const Texture* aTexture);

		static bool CreateComputeBuffer(ComputeBuffer* outBuffer, StructuredBufferType aAccessFlag, UINT aStructSize, UINT aCount, const std::string& aName = "Unnamed Buffer", bool aShouldCreateWithData = false);
		static void UpdateComputeBufferData(const ComputeBuffer& aBuffer);
		static void SetComputeBuffer(UINT aPipelineStages, unsigned aSlot, ComputeBuffer* aBuffer, bool aIsWritable);

		//Special buffers
		static bool CreateAppendConsumeBuffer(ComputeBuffer* outBuffer, UINT aStructSize, UINT aCount, const std::string& aName = "Unnamed Buffer", bool aShouldCreateWithData = false);

		static bool CreateConstantBuffer(ComPtr<ID3D11Buffer>& outCBuffer, size_t aSize, const std::string& aName = "Unnamed Buffer", UINT aBindFlag = D3D11_BIND_CONSTANT_BUFFER);
		static void UpdateConstantBufferData(const ConstantBuffer& aBuffer);
		static void SetConstantBuffer(UINT aPipelineStages, unsigned aSlot, const ConstantBuffer& aBuffer);

		static void SetVertexAndIndexBuffers(const std::vector<ComPtr<ID3D11Buffer>>& aVertexBuffers, const ComPtr<ID3D11Buffer>& aIndexBuffer, const std::vector<unsigned>& aVertexStrides);
		static void SetVertexAndIndexBuffers(const ComPtr<ID3D11Buffer>& aVertexBuffer, const ComPtr<ID3D11Buffer>& aIndexBuffer, unsigned aVertexStrides);
		static void SetInputLayout(const ComPtr<ID3D11InputLayout>& aInputLayout);
		static void SetTopology(unsigned aTopology);

		static void Dispatch(unsigned aX, unsigned aY, unsigned aZ);
		static void Draw(unsigned aCount);
		static void DrawIndexed(unsigned aCount);
		static void DrawIndexedInstanced(unsigned aCount, unsigned aInstanceCount);

		static void CopyRegion(const Texture* aSource, const Texture* aDestination, unsigned X, unsigned Y, unsigned Width, unsigned Height);
		static CU::Vector4b GetSinglePixelValue(const Texture* aSource);
		static CU::Vector4f GetSinglePixelValueFloat(const Texture* aSource);

		static const bool ResizeDevice(Texture* outBackBuffer, Texture* outDepthBuffer);

		static void Present(unsigned aSyncInterval = 0);

		static ComPtr<ID3D11Device>& GetDevice() { return myDevice; }
		static ComPtr<ID3D11DeviceContext>& GetContext() { return myContext; }

	private:
		inline static ComPtr<ID3D11DeviceContext> myContext;
		inline static ComPtr<ID3D11Device> myDevice;
		inline static ComPtr<IDXGISwapChain> mySwapChain;

		inline static std::array<ComPtr<ID3D11RasterizerState>, static_cast<unsigned>(RasterizerState::COUNT)> myRasterizerStates;
		inline static std::array<ComPtr<ID3D11DepthStencilState>, static_cast<unsigned>(DepthState::COUNT)> myDepthStates;

		inline static PipelineStateObject* myLastPSO;
	};
}
