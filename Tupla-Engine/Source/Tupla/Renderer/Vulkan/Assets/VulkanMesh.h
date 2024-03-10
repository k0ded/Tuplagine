#pragma once
#include "VulkanBuffer.h"
#include "Tupla/Renderer/Mesh.h"

namespace Tupla
{
    class VulkanMesh : public Mesh
    {
    public:
        VulkanMesh(VulkanDevice& device): m_Device(device) {}
        void CreateMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) override;

        void Bind(VkCommandBuffer buffer) const;
        void Render(VkCommandBuffer buffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffers(const std::vector<uint32_t>& indicies);
    
        VulkanDevice& m_Device;
        Scope<VulkanBuffer> m_IndexBuffer;
        Scope<VulkanBuffer> m_VertexBuffer;
    };
}
