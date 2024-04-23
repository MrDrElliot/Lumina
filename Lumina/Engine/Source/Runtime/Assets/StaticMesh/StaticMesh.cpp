#include "StaticMesh.h"

#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    LStaticMesh::LStaticMesh(std::filesystem::path InFilePath): LAsset(EAssetType::StaticMesh, InFilePath)
    {
        CreateMesh(InFilePath);
    }

    void LStaticMesh::CreateMesh(std::filesystem::path InFilePath)
    {
        
    }
}
