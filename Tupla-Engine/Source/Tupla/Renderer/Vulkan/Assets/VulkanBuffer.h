#pragma once
#include "Tupla/Renderer/Buffer.h"
#include "Tupla/Renderer/Vulkan/VulkanDevice.h"

namespace Tupla
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(
            VulkanDevice& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~VulkanBuffer() override;
 
        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;

        bool Map(u64 size = VK_WHOLE_SIZE, u64 offset = 0) override;
        void Unmap() override;

        void WriteToBuffer(const void* data, u64 size = VK_WHOLE_SIZE, u64 offset = 0) const override;
        bool Flush(u64 size = VK_WHOLE_SIZE, u64 offset = 0) const override;
        VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

        void WriteToIndex(const void* data, int index) const;
        bool FlushIndex(int index) const;
        VkDescriptorBufferInfo DescriptorInfoForIndex(int index) const;
        VkResult InvalidateIndex(int index) const;

        [[nodiscard]] VkBuffer GetBuffer() const { return buffer; }
        [[nodiscard]] uint32_t GetInstanceCount() const { return instanceCount; }
        [[nodiscard]] VkDeviceSize GetInstanceSize() const { return instanceSize; }
        [[nodiscard]] VkDeviceSize GetAlignmentSize() const { return alignmentSize; }
        [[nodiscard]] VkBufferUsageFlags GetUsageFlags() const { return usageFlags; }
        [[nodiscard]] VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return memoryPropertyFlags; }
        [[nodiscard]] VkDeviceSize GetBufferSize() const { return bufferSize; }

    private:
        static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
 
        VulkanDevice& m_Device;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
 
        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };   
}
