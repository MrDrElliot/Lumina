#pragma once
#include <filesystem>
#include <fastgltf/base64.hpp>
#include <glm/glm.hpp>
#include "Source/Runtime/Assets/Factories/Factory.h"


namespace Lumina
{

    class LStaticMesh;
    struct FMeshSurface
    {
        glm::uint32_t StartIndex;
        glm::uint32_t Count;
    };
    
    class FStaticMeshFactory : public FFactory
    {
    public:

        static std::vector<std::shared_ptr<LStaticMesh>> Import(std::filesystem::path InPath);

        static void ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath);

        

    };
}
