#pragma once

namespace Tupla
{
    class Buffer
    {
    public:
        virtual ~Buffer() = default;
        
        virtual bool Map() = 0;
        virtual void Unmap() = 0;

        template <typename T>
        void WriteToBuffer(const T& aType, u64 offset = 0)
        {
            WriteToBuffer(&aType, sizeof(T), offset);
        }

        [[nodiscard]] void* GetMappedMemory() const { return m_MappedMemory; }
        [[nodiscard]] void* GetGPUMemory() const { return m_GPUMemory; }

    protected:
        virtual void WriteToBuffer(const void* data, u64 size, u64 offset = 0) = 0;

        void* m_GPUMemory = nullptr;
    	void* m_MappedMemory = nullptr;
    };
}
