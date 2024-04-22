//#pragma once
//#include <wrl.h>
//#include <vector>
//#include <memory>
//
//#include "RHIStructs.hpp"
//
//struct ID3D11Device;
//struct ID3D11DeviceContext;
//struct IDXGISwapChain;
//struct ID3D11Buffer;
//struct ID3D11InputLayout;
//struct ID3DUserDefinedAnnotation;
//struct ID3D11DeviceChild;
//
//namespace Tupla
//{
//
//	class ConstantBuffer;
//	class Shader;
//	class Texture;
//
//	class RenderHardwareInterface
//	{
//	public:
//		RenderHardwareInterface();
//		~RenderHardwareInterface();
//
//		bool Initialize(HWND aWindowHandle, bool aEnableDebug);
//		void Destroy();
//		void Present() const;
//
//		template<typename VertexType>
//		bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer);
//
//		bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);
//
//		bool CreateConstantBuffer(std::string_view aName, unsigned aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer);
//
//		void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset = 0) const;
//		void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;
//		void SetConstantBuffer(ConstantBuffer& aBuffer) const;
//
//		template<typename BufferData>
//		bool UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData) const;
//
//		void SetPrimitiveTopology(unsigned aTopology) const;
//
//		bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
//		void SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout) const;
//
//		bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
//
//		void Draw(unsigned aVertexCount);
//		void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const;
//
//		bool ResizeBuffers(const CU::Vector2ui& aNewSize);
//
//		// TEMP
//		void ClearBackBuffer() const;
//		void SetShaders(const Shader& aVxShader, const Shader& aPxShader) const;
//		void SetRenderTarget() const;
//
//
//	private:
//		Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
//		Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
//		Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;
//		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> myUDA;
//
//		std::shared_ptr<Texture> myBackBuffer;
//		std::shared_ptr<Texture> myDepthBuffer;
//
//		bool CreateBackBuffer(const CU::Vector2ui& aBufferSize);
//		bool CreateDepthBuffer(const CU::Vector2ui& aBufferSize);
//
//		bool CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize);
//		bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const;
//
//		void SetObjectName(const Microsoft::WRL::ComPtr<ID3D11DeviceChild>& aObject, std::string_view aName) const;
//	};
//
//	template<typename VertexType>
//	inline bool RenderHardwareInterface::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer)
//	{
//		const size_t vxSize = sizeof(VertexType);
//		const size_t vxCount = aVertexList.size();
//
//		return CreateVertexBufferInternal(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount, vxSize);
//	}
//
//	template<typename BufferData>
//	inline bool RenderHardwareInterface::UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData) const
//	{
//		const size_t dataSize = sizeof(BufferData);
//
//		return UpdateConstantBufferInternal(aBuffer, &aBufferData, dataSize);
//	}
//
//}