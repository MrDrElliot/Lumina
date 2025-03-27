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
        FString FullPath = Paths::ResolveVirtualPath(InRecord->GetAssetPath().GetPathAsString());
        
        if (!FFileHelper::LoadFileToArray(Data, FullPath))
        {
            return ELoadResult::Failed;
        }

        FAssetHeader Header;
        FMemoryReader Reader(Data);
        Reader << Header;

        Assert(Header.Type == EAssetType::Material);
        Assert(Header.Path == InRecord->GetAssetPath());
        
        AMaterial* Material = FMemory::New<AMaterial>();
        Material->SetAssetPath(InRecord->GetAssetPath());
        Material->Serialize(Reader);

        InRecord->SetAssetPtr(Material);

        return ELoadResult::Succeeded;
    }

    FAssetPath FMaterialFactory::CreateNew(const FString& Path)
    {
        FString VirtualPath = Path + "/NewMaterial.lasset";
        FAssetPath NewAsset(VirtualPath);
        
        FString FullPath = Paths::ResolveVirtualPath(VirtualPath);
        
        
        FAssetHeader Header;
        Header.Path = NewAsset;
        Header.Guid = FGuid::Generate();
        Header.Version = 1;
        Header.Type = EAssetType::Material;
        
        if (FFileHelper::CreateNewFile(FullPath, true))
        {
            TVector<uint8> Buffer;
            FMemoryWriter Writer(Buffer);
            Writer << Header;
            
            Assert(FFileHelper::SaveArrayToFile(Buffer, FullPath));


            GEngine->GetEngineSubsystem<FAssetRegistry>()->AssetCreated(NewAsset, Header);
            
            return NewAsset;
        }

        return {};
        
    }
    
}
