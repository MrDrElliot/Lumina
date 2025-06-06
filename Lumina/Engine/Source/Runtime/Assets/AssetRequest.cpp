
#include "AssetRequest.h"

#include "Core/Serialization/MemoryArchiver.h"
#include "Factories/Factory.h"
#include "Paths/Paths.h"
#include "Core/Object/Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    bool FAssetRequest::Process()
    {
        FString FullPath = Paths::ResolveVirtualPath(AssetPath) + ".lasset";

        TVector<uint8> Blob;
        if (!FFileHelper::LoadFileToArray(Blob, FullPath))
        {
            LOG_ERROR("Failed to find file at path: {}", FullPath);
            bFailed = true;
            return false;
        }

        FMemoryReader Reader(Blob);
        
        FAssetHeader Header;
        Reader << Header;
        
        CClass* Class = FindObject<CClass>(UTF8_TO_WIDE(Header.ClassName).c_str());
        
        FString Name = Paths::FileName(AssetPath);
        PendingObject = NewObject<CObject>(Class, UTF8_TO_WIDE(AssetPath).c_str(), FName(Name));

        Class->ForEachProperty([this](FProperty* Prop)
        {
            LOG_ERROR("{}", Prop->Name.c_str());
        });
        
        return true;
    }
}
