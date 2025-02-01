#pragma once

#include <filesystem>
#include <fastgltf/glm_element_traits.hpp>
#include "Containers/Array.h"
#include "Assets/Factories/Factory.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{

    class AStaticMesh;
    struct FMeshSurface
    {
        uint32 StartIndex;
        uint32 Count;
    };
    
    class FStaticMeshFactory : public FFactory
    {
    public:

        ELoadResult CreateNew(const FAssetHandle& InHandle, const FAssetPath& InPath, FAssetRecord* InRecord, FArchive& Archive) override;

        static TVector<TSharedPtr<AStaticMesh>> Import(std::filesystem::path InPath);

        static void ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath);

        

    };
}
