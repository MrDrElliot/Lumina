#include "MaterialFactory.h"

#include "Assets/AssetHeader.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{

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

        TVector<uint8> Buffer;

        FPackageSaver Saver(Buffer);
        Saver << Header;

        CObject* Temp = NewObject<CMaterial>(UTF8_TO_WIDE(VirtualPath).c_str());

        FBinaryStructuredArchive BinaryAr(Saver);
        Temp->Serialize(BinaryAr.Open());
        
        Assert(FFileHelper::SaveArrayToFile(Buffer, FullPath))

        Temp->DestroyNow();
    }
}
