#pragma once
#include <filesystem>
#include <fastgltf/base64.hpp>

#include "Source/Runtime/Assets/Factories/Factory.h"

namespace Lumina
{
    class FStaticMeshFactory : public FFactory
    {
    public:

        void Import(std::filesystem::path InPath);

        void ExtractAsset(fastgltf::Asset* OutAsset, std::filesystem::path InPath);
        

    };
}
