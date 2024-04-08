#pragma once
#include <wrl/client.h>
#include <d3d11.h>

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
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Buffer> GetGPUMemory() const { return m_GPUMemory; }

    protected:
        virtual void WriteToBuffer(const void* data, u64 size, u64 offset = 0) = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_GPUMemory = nullptr;
    	void* m_MappedMemory = nullptr;
    };
}
