﻿#pragma once

namespace Tupla
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual bool SetImageData(void* imageData, u32 texWidth, u32 texHeight, const std::string& debugName = "GenericTexture") = 0;
        virtual void GetImageData(std::vector<std::byte>& data) = 0;
        virtual void* ImGuiRID() = 0;

        bool IsLoaded() const { return m_Loaded.load(); }

    protected:
        std::atomic_bool m_Loaded = false;
    };
}
