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
    
    void CStaticMeshFactory::CreateAssetFile(const FString& Path)
    {
        FString FullPath = Path + ".lasset";
 
    }
}
