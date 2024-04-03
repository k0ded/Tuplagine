#include "tgpch.h"
#include "DX11Mesh.h"

#include "Tupla/Renderer/DX11/DX11Renderer.h"

static u32 stride = sizeof(Tupla::Vertex);
static u32 offset = 0;

Tupla::DX11Mesh::DX11Mesh(DX11Renderer* renderer): m_Renderer(renderer)
{
}

Tupla::DX11Mesh::~DX11Mesh()
{
	m_IndexBuffer->Release();
	m_VertexBuffer->Release();
}

void Tupla::DX11Mesh::CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices)
{
    m_VertexCount = static_cast<u32>(vertices.size());
    m_IndexCount = static_cast<u32>(indices.size());

    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = static_cast<u32>(vertices.size() * sizeof(Vertex)); // in xube.h
    vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices.data() };

    auto result = m_Renderer->GetDevice()->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &m_VertexBuffer);
    ASSERT(SUCCEEDED(result), "Failed to create vertex buffer");

    D3D11_BUFFER_DESC indexbufferdesc = {};
    indexbufferdesc.ByteWidth = static_cast<u32>(indices.size() * sizeof(u32)); // in xube.h
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexbufferSRD = { indices.data() };

    result = m_Renderer->GetDevice()->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &m_IndexBuffer);
    ASSERT(SUCCEEDED(result), "Failed to create index buffer");
}

void Tupla::DX11Mesh::AttachMesh()
{
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}
