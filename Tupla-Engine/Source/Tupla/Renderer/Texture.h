#pragma once

namespace Tupla
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual void SetImageData(void* imageData, u32 texWidth, u32 texHeight, u32 texBytesPerPixel) = 0;
        virtual void* ImGuiRID() = 0;
    };
}
