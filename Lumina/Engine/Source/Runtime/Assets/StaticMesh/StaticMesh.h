#pragma once
#include <filesystem>

#include "Source/Runtime/Assets/Asset.h"

namespace Lumina
{
    class FBuffer;

    struct FMeshAsset
    {
        std::string Name;

        std::shared_ptr<FBuffer> VertexBuffer;
        std::shared_ptr<FBuffer> IndexBuffer;
    };
    

    class LStaticMesh : public LAsset
    {
    public:
        
        LStaticMesh(std::filesystem::path InFilePath);
        void CreateMesh(std::filesystem::path InFilePath);

    private:

        std::vector<FMeshAsset> Meshes;

        std::shared_ptr<FBuffer> VBO;
        std::shared_ptr<FBuffer> IBO;
    };
}
