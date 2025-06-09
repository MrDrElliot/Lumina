
#include "AssetRequest.h"

#include "Core/Serialization/MemoryArchiver.h"
#include "Factories/Factory.h"
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

        
        Class->ForEachProperty<FEnumProperty>([this](FEnumProperty* Prop)
        {
            LOG_ERROR("THIS IS A PROPERTY {}", Prop->Name.c_str());

            CEnum* Enum = Prop->GetEnum();
            Enum->ForEachEnum([] (const TPair<FName, uint64>& Pair)
            {
                LOG_ERROR("ENUMS: {}", Pair.first.c_str());
            });
        });
        
        return true;
    }
}
