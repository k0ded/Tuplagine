#pragma once

namespace Tupla
{
#define BUFFER_MAX_SIZE (~1ull)
    
    class Buffer
    {
    public:
        virtual ~Buffer() = default;
        
        virtual bool Map(u64 size = BUFFER_MAX_SIZE, u64 offset = 0) = 0;
        virtual void Unmap() = 0;

        virtual void WriteToBuffer(const void* data, u64 size = BUFFER_MAX_SIZE, u64 offset = 0) const = 0;
        virtual bool Flush(u64 size = BUFFER_MAX_SIZE, u64 offset = 0) const = 0;

        [[nodiscard]] void* GetMappedMemory() const { return m_MappedMemory; }

    protected:
        void* m_MappedMemory = nullptr;
    };
}
