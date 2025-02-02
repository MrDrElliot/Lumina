#pragma once

#include "AssetTypes.h"
#include "Containers/String.h"
#include "Core/Serialization/Archiver.h"


namespace Lumina
{
    class IAsset
    {
    public:

        IAsset() =          default;
        virtual ~IAsset() = default;
        
        virtual EAssetType GetAssetType()   const = 0;
        virtual int32 GetAssetVersion()     const = 0;
        virtual FString GetFriendlyName()   const = 0;
        
        virtual void Serialize(FArchive& Ar) = 0;

        
        
    private:
        
    };
}

#define DECLARE_ASSET(FriendlyName, Type, Version) \
static EAssetType StaticGetAssetType() { static const EAssetType typeID = EAssetType::Type; return typeID; } \
virtual EAssetType GetAssetType() const override { return StaticGetAssetType(); } \
\
static int32 StaticGetAssetVersion() { static const int32 TypeVersion = Version; return TypeVersion; } \
virtual int32 GetAssetVersion() const { return StaticGetAssetVersion(); } \
\
static FString StaticGetFriendlyName() { static const FString TypeName = FriendlyName; return TypeName; } \
virtual FString GetFriendlyName() const { return StaticGetFriendlyName(); } \


#define DEFINE_ASSET_HEADER() \
FAssetHeader Header;

#define ADD_DEPENDENCY(AssetHandle) \
Header.Dependencies.emplace_back(AssetHandle);

#define SERIALIZE_HEADER(Ar) \
Ar << Header;


   


