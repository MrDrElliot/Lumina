
#include "AssetRequest.h"

#include "AssetHeader.h"
#include "Core/Object/Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"
#include "Core/Serialization/Package/PackageLoader.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    bool FAssetRequest::Process()
    {
        FAssetHeader Header;
        
        FString FullPath = Paths::ResolveVirtualPath(AssetPath) + ".lasset";
        FString Name = Paths::FileName(AssetPath);

        TVector<uint8> Buffer;
        if (!FFileHelper::LoadFileToArray(Buffer, FullPath))
        {
            LOG_ERROR("Cannot find asset file at: {}", FullPath);
            bFailed = true;
            return false;
        }

        FPackageLoader Loader(Buffer);
        Loader << Header;

        if (Header.ClassName.empty())
        {
            LOG_ERROR("Corrupted Asset: {}", Header.Path);
            return false;
        }
        
        CClass* Class = FindObject<CClass>(UTF8_TO_WIDE(Header.ClassName).c_str());
        Assert(Class != nullptr)
        
        PendingObject = NewObject(Class, UTF8_TO_WIDE(AssetPath).c_str(), FName(Name));

        FBinaryStructuredArchive BinaryArchive(Loader);
        FArchiveSlot Slot = BinaryArchive.Open();

        PendingObject->Serialize(Slot);
        
        return true;
    }
}
 