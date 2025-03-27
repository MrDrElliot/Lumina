#include "Asset.h"

#include "AssetHeader.h"
#include "AssetRegistry/AssetRegistry.h"
#include "Containers/Array.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"


namespace Lumina
{
    void IAsset::Save()
    {
        FAssetRegistry* Registry = GEngine->GetEngineSubsystem<FAssetRegistry>();
        bool bSuccess;
        FAssetHeader Header = Registry->FindAssetHeader(AssetPath, &bSuccess);
        
        if (bSuccess)
        {
            TVector<uint8> Blob;
            FMemoryWriter Writer(Blob);
            Writer << Header;
            Serialize(Writer);

            FString FullPath = Paths::ResolveVirtualPath(AssetPath.GetPathAsString());
            Assert(FFileHelper::SaveArrayToFile(Blob, FullPath));
        }
    }
}
