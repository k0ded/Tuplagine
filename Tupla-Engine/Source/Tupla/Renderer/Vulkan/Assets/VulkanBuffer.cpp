#include "tgpch.h"
#include "VulkanBuffer.h"

namespace Tupla
{
    /**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
    VkDeviceSize VulkanBuffer::GetAlignment(const VkDeviceSize instanceSize, const VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    VulkanBuffer::VulkanBuffer(
        VulkanDevice& device,
        const VkDeviceSize instanceSize,
        const uint32_t instanceCount,
        const VkBufferUsageFlags usageFlags,
        const VkMemoryPropertyFlags memoryPropertyFlags,
        const VkDeviceSize minOffsetAlignment)
        : m_Device{device},
          instanceCount{instanceCount},
          instanceSize{instanceSize},
          usageFlags{usageFlags},
          memoryPropertyFlags{memoryPropertyFlags}
    {
        alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Unmap();
        vkDestroyBuffer(m_Device.Device(), buffer, nullptr);
        vkFreeMemory(m_Device.Device(), memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    bool VulkanBuffer::Map(const VkDeviceSize size, const VkDeviceSize offset)
    {
        assert(buffer && memory && "Called map on buffer before create");
        return vkMapMemory(m_Device.Device(), memory, offset, size, 0, &m_MappedMemory);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void VulkanBuffer::Unmap()
    {
        if (m_MappedMemory)
        {
            vkUnmapMemory(m_Device.Device(), memory);
            m_MappedMemory = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void VulkanBuffer::WriteToBuffer(const void* data, const VkDeviceSize size, const VkDeviceSize offset) const
    {
        assert(m_MappedMemory && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(m_MappedMemory, data, bufferSize);
        }
        else
        {
            auto memOffset = static_cast<char*>(m_MappedMemory);
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return If the flush call was successful or not
     */
    bool VulkanBuffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_Device.Device(), 1, &mappedRange) == VK_SUCCESS;
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult VulkanBuffer::Invalidate(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_Device.Device(), 1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo VulkanBuffer::DescriptorInfo(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        return VkDescriptorBufferInfo{
            buffer,
            offset,
            size,
        };
    }

    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void VulkanBuffer::WriteToIndex(const void* data, const int index) const
    {
        WriteToBuffer(data, instanceSize, index * alignmentSize);
    }

    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    bool VulkanBuffer::FlushIndex(const int index) const { return Flush(alignmentSize, index * alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(const int index) const
    {
        return DescriptorInfo(alignmentSize, index * alignmentSize);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult VulkanBuffer::InvalidateIndex(const int index) const
    {
        return Invalidate(alignmentSize, index * alignmentSize);
    }
}
