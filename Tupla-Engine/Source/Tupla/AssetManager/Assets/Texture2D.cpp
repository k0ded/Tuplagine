#include "tgpch.h"
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
        m_Texture->SetImageData(pixels, texWidth, texHeight, 4);
        stbi_image_free(pixels);
    }

    std::string Texture2D::SerializeAsset()
    {
        return "EMPTY SERIALIZATION ATM";
    }

    void Texture2D::DeserializeAssetPacked(const std::string& data)
    {
    }

    std::string Texture2D::SerializeAssetPacked()
    {
        return "EMPTY SERIALIZATION ATM";
    }
}
