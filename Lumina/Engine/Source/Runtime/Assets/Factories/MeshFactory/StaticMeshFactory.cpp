#include "StaticMeshFactory.h"

#include <random>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include "Renderer/RHIIncl.h"
#include "Log/Log.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"


namespace Lumina
{

    CObject* CStaticMeshFactory::CreateNew(const FString& Path)
    {
        return {};
    }

    void CStaticMeshFactory::CreateAssetFile(const FString& Path)
    {
        CFactory::CreateAssetFile(Path);
    }
}
