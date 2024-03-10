#include "tgpch.h"
#include "VulkanMesh.h"

namespace Tupla
{
    void VulkanMesh::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
    {
        CreateVertexBuffers(vertices);
        CreateIndexBuffers(indices);
    }

    void VulkanMesh::Bind(const VkCommandBuffer buffer) const
    {
        const VkBuffer buffers[] = { m_VertexBuffer->GetBuffer() };
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, buffers, offsets);

        if(m_HasIndexBuffer)
        {
            vkCmdBindIndexBuffer(buffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkanMesh::Render(const VkCommandBuffer buffer)
    {
        if(m_HasIndexBuffer)
        {
            vkCmdDrawIndexed(buffer, m_IndexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(buffer, m_VertexCount, 1, 0, 0);
        }
    }

    void VulkanMesh::CreateVertexBuffers(const std::vector<Vertex>& vertices)
    {
        m_VertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        VulkanBuffer staging {
            m_Device,
            vertexSize,
            m_VertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        staging.Map();
        staging.WriteToBuffer(vertices.data());

        m_VertexBuffer = CreateScope<VulkanBuffer>(
            m_Device,
            vertexSize,
            m_VertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

        m_Device.CopyBuffer(staging.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
    }

    void VulkanMesh::CreateIndexBuffers(const std::vector<uint32_t>& indicies)
    {
        m_IndexCount = static_cast<uint32_t>(indicies.size());
        m_HasIndexBuffer = m_IndexCount > 0;

        if(!m_HasIndexBuffer) return;

        constexpr uint32_t indexSize = sizeof(indicies[0]);
        const VkDeviceSize bufferSize = sizeof(indicies[0]) * m_IndexCount;

        VulkanBuffer staging {
            m_Device,
            indexSize,
            m_IndexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        staging.Map();
        staging.WriteToBuffer(indicies.data());

        m_IndexBuffer = CreateScope<VulkanBuffer>(
            m_Device,
            indexSize,
            m_IndexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

        m_Device.CopyBuffer(staging.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
    }
}
