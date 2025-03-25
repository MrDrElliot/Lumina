#pragma once

#include "AssetPath.h"
#include "AssetTypes.h"
#include "Containers/String.h"
#include "Core/Serialization/Archiver.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class IAsset
    {
    public:

        IAsset() :AssetPath(FAssetPath()) {}
        IAsset(const FAssetPath& InPath) :AssetPath(InPath) {}
        virtual ~IAsset() = default;

        
        virtual EAssetType GetAssetType()   const = 0;
        virtual int32 GetAssetVersion()     const = 0;
        virtual FString GetFriendlyName()   const = 0;

        FORCEINLINE const FAssetPath& GetAssetPath() const { return AssetPath; }
        FORCEINLINE bool HasValidPath() const { return AssetPath.IsValid(); }

        void SetAssetPath(const FAssetPath& InPath) { AssetPath = InPath; }

        void Save();
        
        /** Operates for reading and writing serialization, depending which mode the archiver is in. */
        virtual void Serialize(FArchive& Ar) { AssertMsg(0, "Unimplemented!"); }

        /** Called after factory serialization to give the asset a chance to load resource from deserialized data. */
        virtual void PostLoad() { }

        /** Called after all dependencies are loaded. */
        virtual void PostLoadDependencies() { }

    
    private:

        /** Can be used for identifying this asset */
        FAssetPath          AssetPath;
        
    };
}

//-----------------------------------------------------
// Macro definitions for ease-of-use.
//-----------------------------------------------------

#define DECLARE_ASSET(FriendlyName, Type, Version) \
static EAssetType StaticGetAssetType() { static const EAssetType typeID = EAssetType::Type; return typeID; } \
virtual EAssetType GetAssetType() const override { return StaticGetAssetType(); } \
\
static int32 StaticGetAssetVersion() { static constexpr int32 TypeVersion = Version; return TypeVersion; } \
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


   


