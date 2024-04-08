#pragma once

namespace Tupla
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual bool SetImageData(void* imageData, u32 texWidth, u32 texHeight) = 0;
        virtual u64 GetImageData(std::byte** data) = 0;
        virtual void* ImGuiRID() = 0;
    };
}
