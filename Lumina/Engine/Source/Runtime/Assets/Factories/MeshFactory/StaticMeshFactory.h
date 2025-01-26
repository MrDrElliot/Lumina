#pragma once

#include <filesystem>
#include <fastgltf/glm_element_traits.hpp>
#include "Containers/Array.h"
#include "Assets/Factories/Factory.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{

    class LStaticMesh;
    struct FMeshSurface
    {
        uint32 StartIndex;
        uint32 Count;
    };
    
    class FStaticMeshFactory : public FFactory
    {
    public:

        std::shared_ptr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) override;

        static TArray<std::shared_ptr<LStaticMesh>> Import(std::filesystem::path InPath);

        static void ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath);

        

    };
}
