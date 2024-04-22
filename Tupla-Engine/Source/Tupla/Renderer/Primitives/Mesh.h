#pragma once
#include <wrl.h>
#include "../Primitives/Vertex.h"

namespace Tupla
{
	class Mesh
	{
	public:
		Mesh(std::string aName);
		~Mesh();

		void CreateMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);

		std::string myName;

		bool myIsSkinned;
		u32 myLOD;
		i32 mySubMesh;

		u32 myVertexCount;
		u32 myIndexCount;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myVXBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> myIXBuffer;
	};
}
