#pragma once

#include "AssetPath.h"
#include "Containers/Array.h"
#include "GUID/GUID.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FAssetHandle;
    enum class EAssetType : uint8;

    struct LUMINA_API FAssetHeader
    {
        
        int32                       Version = -1;
        FString                     ClassName;
        EAssetType                  Type;
        FAssetPath                  Path;

        friend FArchive& operator << (FArchive& Ar, FAssetHeader& Data)
        {
            Ar << Data.Version;
            Ar << Data.ClassName;
            Ar << Data.Type;
            Ar << Data.Path;

            return Ar;
        }
    };
}
