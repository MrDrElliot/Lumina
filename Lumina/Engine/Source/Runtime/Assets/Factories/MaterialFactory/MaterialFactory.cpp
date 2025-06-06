#include "MaterialFactory.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"

namespace Lumina
{
    
    CObject* CMaterialFactory::CreateNew(const FString& Path)
    {
        return NewObject<CMaterial>();
    }

    void CMaterialFactory::CreateAssetFile(const FString& Path)
    {
        FString FullPath = Path + ".lasset";
        if (!FFileHelper::CreateNewFile(FullPath, true))
        {
            LOG_INFO("Failed to created New Material: {}", FullPath);
            return;
        }

        FAssetHeader Header;
        Header.Path = FAssetPath(Paths::ConvertToVirtualPath(Path));
        Header.ClassName = "CMaterial";
        Header.Type = EAssetType::Material;
        Header.Version = 1;
        Header.Guid = FGuid::Generate();

        TVector<uint8> Blob;
        FMemoryWriter Writer(Blob);
        Writer << Header;
        
        FFileHelper::SaveArrayToFile(Blob, FullPath);
        
    }
}
