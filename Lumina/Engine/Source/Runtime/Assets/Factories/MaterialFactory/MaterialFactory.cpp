#include "MaterialFactory.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"

namespace Lumina
{
    
    CObject* CMaterialFactory::CreateNew(const FString& Path)
    {
        return NewObject<CMaterial>(UTF8_TO_WIDE(Path).c_str());
    }

    void CMaterialFactory::CreateAssetFile(const FString& Path)
    {
        FString FullPath = Path + ".lasset";
        if (!FFileHelper::CreateNewFile(FullPath, true))
        {
            LOG_INFO("Failed to created New Material: {}", FullPath);
            return;
        }

        FString VirtualPath = Paths::ConvertToVirtualPath(Path);

        FAssetHeader Header;
        Header.Path = VirtualPath;
        Header.ClassName = "CMaterial";
        Header.Type = EAssetType::Material;
        Header.Version = 1;
        Header.Guid = FGuid::Generate();

        TVector<uint8> Buffer;

        FPackageSaver Saver(Buffer);
        Saver << Header;

        CObject* Temp = CreateNew(VirtualPath);

        FBinaryStructuredArchive BinaryAr(Saver);
        Temp->Serialize(BinaryAr.Open());
        
        FFileHelper::SaveArrayToFile(Buffer, FullPath);

        Memory::Delete(Temp);
    }
}
