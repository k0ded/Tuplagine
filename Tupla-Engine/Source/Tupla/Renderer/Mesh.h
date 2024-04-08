#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <CommonUtilities/Math/Vector/Vector.h>

struct ID3D11InputLayout;

namespace Tupla
{
    struct Vertex
    {
        CU::Vector3f Position {};
        CU::Vector3f Normal{};
        CU::Vector2f UV{};
        CU::Vector3f Color{};

        bool operator==(const Vertex& other) const
        {
            return Position == other.Position && Color == other.Color && Normal == other.Normal && UV == other.UV;
        }

        constexpr static D3D11_INPUT_ELEMENT_DESC InputDescription[] =
        {
	        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 position
	        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 normal
	        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float2 texcoord
	        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color
        };
    };
    
    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void CreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices) = 0;
        virtual void AttachMesh() = 0;

        [[nodiscard]] u32 GetVertexCount() const { return m_VertexCount; }
        [[nodiscard]] u32 GetIndexCount() const { return m_IndexCount; }
        [[nodiscard]] u32 GetTriCount() const { return m_IndexCount / 3; }
    protected:
        u32 m_VertexCount {};
        u32 m_IndexCount {};
    };
}
