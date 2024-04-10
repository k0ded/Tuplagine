#pragma once
#include "Tupla/Renderer/Mesh.h"

namespace Tupla
{
	class DX11Renderer;
}

namespace Tupla
{
	class DX11Mesh : public Mesh
	{
	public:
		DX11Mesh(DX11Renderer* renderer);
		~DX11Mesh() override;
		void CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, const std::string& debugName = "GenericMesh") override;
		void CreateMesh(const std::byte* aData, u64 dataSize, const std::string& debugName = "GenericMesh") override;
		bool AttachMesh() override;

		void AppendMeshData(std::vector<std::byte>& outResult) override;

	private:
		DX11Renderer* m_Renderer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer{};
	};
}
