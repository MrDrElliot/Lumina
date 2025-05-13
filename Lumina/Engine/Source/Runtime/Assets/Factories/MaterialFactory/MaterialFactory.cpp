#include "MaterialFactory.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"

namespace Lumina
{
    
    FAssetPath FMaterialFactory::CreateNew(const FString& Path)
    {
        return {};   
    }
    
}
