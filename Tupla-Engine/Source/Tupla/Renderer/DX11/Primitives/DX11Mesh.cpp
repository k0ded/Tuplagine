#include "tgpch.h"
#include "DX11Mesh.h"

#include "Tupla/Core/Application.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

static u32 stride = sizeof(Tupla::Vertex);
static u32 offset = 0;

Tupla::DX11Mesh::DX11Mesh(DX11Renderer* renderer): m_Renderer(renderer)
{
}

Tupla::DX11Mesh::~DX11Mesh() = default;

void Tupla::DX11Mesh::CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, const std::string& debugName)
{
    m_VertexCount = static_cast<u32>(vertices.size());
    m_IndexCount = static_cast<u32>(indices.size());

    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = static_cast<u32>(vertices.size() * sizeof(Vertex));
    vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices.data() };

    auto result = m_Renderer->GetDevice()->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &m_VertexBuffer);
    ASSERT(SUCCEEDED(result), "Failed to create vertex buffer");
    DX11Renderer::SetObjectName(m_VertexBuffer.Get(), (debugName + "_VB").c_str());

    if (indices.empty()) return;

    D3D11_BUFFER_DESC indexbufferdesc = {};
    indexbufferdesc.ByteWidth = static_cast<u32>(indices.size() * sizeof(u32));
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexbufferSRD = { indices.data() };

    result = m_Renderer->GetDevice()->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &m_IndexBuffer);
    ASSERT(SUCCEEDED(result), "Failed to create index buffer");
    DX11Renderer::SetObjectName(m_IndexBuffer.Get(), (debugName + "_IB").c_str());
}

void Tupla::DX11Mesh::CreateMesh(const std::byte* aData, u64 dataSize, const std::string& debugName)
{
    if(dataSize < 8)
    {
        LOG_ERROR("INVALID MESH DATA!");
        return;
    }

    const u32 vertexCount = *(u32*)aData;
    const u32 indexCount = *(u32*)(aData + 4);

    u64 ptr = 8;

    std::vector<Vertex> vertices(vertexCount);

    for (u32 i = 0; i < vertexCount; ++i)
    {
        if(dataSize < ptr)
        {
            LOG_ERROR("MESH DATA CORRUPT!");
            return;
        }
        vertices[i] = *(Vertex*)(aData + ptr);
        ptr += sizeof(Vertex);
    }

    std::vector<u32> indices(indexCount);
    for (u32 i = 0; i < indexCount; ++i)
    {
        if (dataSize < ptr)
        {
            LOG_ERROR("MESH DATA CORRUPT!");
            return;
        }

        indices[i] = *(u32*)(aData + ptr);
        ptr += sizeof(u32);
    }

    CreateMesh(vertices, indices, debugName);
}

bool Tupla::DX11Mesh::AttachMesh()
{
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

    if(m_IndexCount > 0)
    {
        m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    }
    return true;
}


// Mesh data
// Vertex count		- 4 bytes
// Index count		- 4 bytes
// Vertex data		- xxx bytes
// Index data		- xxx bytes
void Tupla::DX11Mesh::AppendMeshData(std::vector<std::byte>& outResult)
{
    u64 seekStart = outResult.size();
    u64 seekPosition = seekStart + 8;
    outResult.resize(seekStart + 8);

    u32 vertexSize = 0;
    u32 indexSize = 0;

    DX11Renderer& renderer = *(DX11Renderer*)Application::Get().GetRenderer();

    if(!m_VertexBuffer)
    {
        memcpy(&outResult[seekStart], &vertexSize, sizeof(u32));
        memcpy(&outResult[seekStart + 4], &indexSize, sizeof(u32));
        return;
    }

    // Vertex data and count
    D3D11_BUFFER_DESC vertexDesc{};
    m_VertexBuffer->GetDesc(&vertexDesc);
    vertexDesc.Usage = D3D11_USAGE_STAGING;
    vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    vertexDesc.BindFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexStaging;
    renderer.GetDevice()->CreateBuffer(&vertexDesc, nullptr, vertexStaging.GetAddressOf());
    renderer.GetDeviceContext()->CopyResource(vertexStaging.Get(), m_VertexBuffer.Get());

    // Vertex count
    vertexSize = vertexDesc.ByteWidth / sizeof(Vertex);
    outResult.resize(seekStart + 8 + vertexSize * sizeof(Vertex));

    // Vertex data
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    renderer.GetDeviceContext()->Map(vertexStaging.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    memcpy_s(&outResult[seekPosition], vertexSize * sizeof(Vertex), mappedResource.pData, vertexDesc.ByteWidth);
    renderer.GetDeviceContext()->Unmap(vertexStaging.Get(), 0);

    seekPosition += vertexSize * sizeof(Vertex);

    if (!m_IndexBuffer)
    {
        memcpy(&outResult[seekStart], &vertexSize, sizeof(u32));
        memcpy(&outResult[seekStart + 4], &indexSize, sizeof(u32));
        return;
    }

    // Index data and count
    D3D11_BUFFER_DESC indexDesc{};
    m_IndexBuffer->GetDesc(&indexDesc);
    indexDesc.Usage = D3D11_USAGE_STAGING;
    indexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    indexDesc.BindFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexStaging;
    renderer.GetDevice()->CreateBuffer(&indexDesc, nullptr, indexStaging.GetAddressOf());
    renderer.GetDeviceContext()->CopyResource(indexStaging.Get(), m_IndexBuffer.Get());

    // Index count
    indexSize = indexDesc.ByteWidth / sizeof(u32);
    outResult.resize(seekPosition + indexSize * sizeof(u32));

    // Index data
    renderer.GetDeviceContext()->Map(indexStaging.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    memcpy_s(&outResult[seekPosition], indexSize * sizeof(u32), mappedResource.pData, indexDesc.ByteWidth);
    renderer.GetDeviceContext()->Unmap(indexStaging.Get(), 0);

    // Set sizes!
    memcpy(&outResult[seekStart], &vertexSize, sizeof(u32));
    memcpy(&outResult[seekStart + 4], &indexSize, sizeof(u32));
}
