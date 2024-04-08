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
		void CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices) override;
		void AttachMesh() override;

	private:
		DX11Renderer* m_Renderer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer{};
	};
}
