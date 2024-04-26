#include "TextureFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Lumina
{
    std::vector<std::uint8_t> FTextureFactory::ImportFromSource(std::filesystem::path Path)
    {
        stbi_set_flip_vertically_on_load(true);
        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);
        std::vector<std::uint8_t> out(data, data + (x * y * STBI_rgb_alpha));
        delete data;
        return out;
    }

    void FTextureFactory::ImportFromSource(std::vector<std::uint8_t>* Out, std::filesystem::path Path)
    {
        stbi_set_flip_vertically_on_load(true);
        FImageSourceMetadata* metadata = (FImageSourceMetadata*)GetMetadata(Path);

        Out->resize(metadata->Width * metadata->Height * metadata->SourceChannels);

        int x, y, c;
        stbi_uc* data = stbi_load(Path.string().c_str(), &x, &y, &c, STBI_rgb_alpha);

        memcpy(Out->data(), data, Out->size());

        delete data;
        delete metadata;
    }

    void FTextureFactory::ImportFromMemory(std::vector<std::uint8_t>* Out, const std::vector<std::uint8_t>& In)
    {
        stbi_set_flip_vertically_on_load(true);
        auto* metadata = GetMetadataFromMemory(In);

        Out->resize(metadata->Width * metadata->Height * 4);
        int x, y, c;
        stbi_uc* data = stbi_load_from_memory(In.data(), In.size(), &x, &y, &c, STBI_rgb_alpha);
        memcpy(Out->data(), data, Out->size());
        delete data;
        delete metadata;
    }

    FImageSourceMetadata* FTextureFactory::GetMetadata(std::filesystem::path Path)
    {
        FImageSourceMetadata* data = new FImageSourceMetadata;
        stbi_info(Path.string().c_str(), &data->Width, &data->Height, &data->SourceChannels);
        return data;
    }

    FImageSourceMetadata* FTextureFactory::GetMetadataFromMemory(const std::vector<std::uint8_t>& In)
    {
        FImageSourceMetadata* data = new FImageSourceMetadata;
        stbi_info_from_memory(In.data(), In.size(), &data->Width, &data->Height, &data->SourceChannels);
        return data;
    }
}
