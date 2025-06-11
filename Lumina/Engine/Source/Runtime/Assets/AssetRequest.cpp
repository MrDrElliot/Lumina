
#include "AssetRequest.h"

#include "Core/Serialization/MemoryArchiver.h"
#include "Paths/Paths.h"
#include "Core/Object/Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    bool FAssetRequest::Process()
    {
        FString FullPath = Paths::ResolveVirtualPath(AssetPath) + ".lasset";
        FString Name = Paths::FileName(AssetPath);

        TVector<uint8> Blob;
        if (!FFileHelper::LoadFileToArray(Blob, FullPath))
        {
            LOG_ERROR("Cannot find asset file at: {}", FullPath);
            bFailed = true;
            return false;
        }

        FMemoryReader Reader(Blob);
        
        FAssetHeader Header;
        Reader << Header;
        
        CClass* Class = FindObject<CClass>(UTF8_TO_WIDE(Header.ClassName).c_str());
        Assert(Class != nullptr)
        
        PendingObject = NewObject(Class, UTF8_TO_WIDE(AssetPath).c_str(), FName(Name));

        FBinaryStructuredArchive BinaryArchive(Reader);
        FArchiveSlot Slot = BinaryArchive.Open();

        PendingObject->Serialize(Slot);
        
        return true;
    }
}
 