#include "tgpch.h"

#include "Tupla/Core/Application.h"
#include "Mesh.h"
#include "d3d11.h"

Tupla::Mesh::Mesh(std::string aName): myName(std::move(aName)), myIsSkinned(false), myLOD(0), mySubMesh(0),
                                      myVertexCount(0), myIndexCount(0)
{
}

Tupla::Mesh::~Mesh() = default;

void Tupla::Mesh::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
{
	RHI::CreateVertexBuffer(myVXBuffer, vertices);
	RHI::CreateIndexBuffer(myIXBuffer, indices);

	myVertexCount = static_cast<u32>(vertices.size());
	myIndexCount = static_cast<u32>(indices.size());
}
