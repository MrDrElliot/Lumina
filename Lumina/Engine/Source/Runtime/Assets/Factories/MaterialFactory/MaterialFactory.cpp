#include "MaterialFactory.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{

    ELoadResult FMaterialFactory::LoadFromDisk(FAssetRecord* InRecord)
    {
        return {};
    }

    FAssetPath FMaterialFactory::CreateNew(const FString& Path)
    {
        FAssetHeader Header;
        Header.Path = Path;
        Header.Guid = FGuid::Generate();
        Header.Version = 1;
        Header.Type = EAssetType::Material;

        if (FFileHelper::CreateNewFile(Path, true))
        {
            TVector<uint8> Buffer;
            FMemoryWriter Writer(Buffer);
            Writer << Header;
            
            Assert(FFileHelper::SaveArrayToFile(Buffer, Path));

            FAssetPath NewAsset(Path);

            GEngine->GetEngineSubsystem<FAssetRegistry>()->AssetCreated(NewAsset, Header);
            
            return NewAsset;
        }

        return {};
        
    }
    
}
