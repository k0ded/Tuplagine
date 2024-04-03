﻿#include "tgpch.h"
#include "Texture2D.h"

#include "stb/stb_image.h"
#include "Tupla/Core/Application.h"

namespace Tupla
{
    void Texture2D::DeserializeAsset(const std::string& aFilePath)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(aFilePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if(!pixels)
        {
            throw std::runtime_error("Failed to load texture image!");
        }

        m_Texture = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
        m_Texture->SetImageData(pixels, texWidth, texHeight);
        stbi_image_free(pixels);
    }

    u64 Texture2D::SerializeAsset(std::byte** outResult)
    {
        if(m_Texture)
        {
			return m_Texture->GetImageData(outResult);
        }
        return 0;
    }

    void Texture2D::DeserializeAssetPacked(const std::byte* data, u64 dataSize)
    {
        ASSERT(dataSize < 5, "INVALID PACKED TEXTURE FORMAT");

        // For specification see DX11Texture.cpp

    	u16 width;
    	u16 height;

        memcpy(&width, &data[0], sizeof(u16));
        memcpy(&height, &data[2], sizeof(u16));

    	// Image data after this block
        m_Texture->SetImageData((void*)&data[4], width, height);
    }

    u64 Texture2D::SerializeAssetPacked(std::byte** outResult)
    {
        if(m_Texture)
        {
            return m_Texture->GetImageData(outResult);
        }
        return 0;
    }
}
