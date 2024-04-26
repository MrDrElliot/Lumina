#pragma once
#include "Source/Runtime/Assets/Factories/Factory.h"
#include <glm/glm.hpp>
#include <filesystem>

namespace Lumina
{

    struct FImageSourceMetadata
    {
        glm::int32 Width;
        glm::int32 Height;
        glm::int32 SourceChannels;
    };
    
    class FTextureFactory : public FFactory
    {
    public:

        static std::vector<std::uint8_t> ImportFromSource(std::filesystem::path Path);
        static void ImportFromSource(std::vector<std::uint8_t>* Out, std::filesystem::path Path);
        static void ImportFromMemory(std::vector<std::uint8_t>* Out, const std::vector<std::uint8_t>& In);
        static FImageSourceMetadata* GetMetadata(std::filesystem::path Path);
        static FImageSourceMetadata* GetMetadataFromMemory(const std::vector<std::uint8_t>& In);

        
    
    };
}
