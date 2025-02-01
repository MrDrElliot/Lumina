#pragma once
#include "AssetPath.h"
#include "Containers/Array.h"
#include "GUID/GUID.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{
    enum class EAssetType : uint8;

    struct FAssetHeader
    {
        
        int32                   Version = -1;
        FGuid                   Guid;
        EAssetType              Type;
        TVector<FGuid>          Dependencies;
        FAssetPath              Path;

        friend FArchive& operator << (FArchive& Ar, FAssetHeader& data)
        {
            Ar << data.Version;
            Ar << data.Guid;
            Ar << data.Type;
            Ar << data.Dependencies;
            Ar << data.Path;

            return Ar;
        }
    };
}
