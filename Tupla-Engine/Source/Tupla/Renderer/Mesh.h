#pragma once
#include <CommonUtilities/Math/Vector/Vector.h>

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
