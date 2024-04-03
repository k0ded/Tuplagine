#pragma once
#include <d3d11.h>

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
		~DX11Mesh();
		void CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices) override;
		void AttachMesh() override;

	private:
		DX11Renderer* m_Renderer;
		ID3D11Buffer* m_VertexBuffer{};
		ID3D11Buffer* m_IndexBuffer{};
	};
}
