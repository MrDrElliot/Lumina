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

    ELoadResult FMaterialFactory::LoadFromDisk(FAssetRecord* InRecord)
    {
        TVector<uint8> Data;
        if (!FFileHelper::LoadFileToArray(Data, InRecord->GetAssetPath().GetPathAsString()))
        {
            return ELoadResult::Failed;
        }
        
        FMemoryReader Reader(Data);
        AMaterial* Material = FMemory::New<AMaterial>();
        Material->SetAssetPath(InRecord->GetAssetPath());
        Material->Serialize(Reader);

        InRecord->SetAssetPtr(Material);

        return ELoadResult::Succeeded;
    }

    FAssetPath FMaterialFactory::CreateNew(const FString& Path)
    {
        FAssetHeader Header;
        Header.Path = Path;
        Header.Guid = FGuid::Generate();
        Header.Version = 1;
        Header.Type = EAssetType::Material;

        FString FullPathName = Paths::CombinePaths(FProject::Get()->GetProjectContentDirectory(), Path);

        if (FFileHelper::CreateNewFile(FullPathName, true))
        {
            TVector<uint8> Buffer;
            FMemoryWriter Writer(Buffer);
            Writer << Header;
            
            Assert(FFileHelper::SaveArrayToFile(Buffer, FullPathName));

            FAssetPath NewAsset(Path);

            GEngine->GetEngineSubsystem<FAssetRegistry>()->AssetCreated(NewAsset, Header);
            
            return NewAsset;
        }

        return {};
        
    }
    
}
