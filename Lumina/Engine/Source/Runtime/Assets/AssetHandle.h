#pragma once

#include "AssetTypes.h"
#include "AssetHeader.h"
#include "AssetRecord.h"
#include "Core/Application/Application.h"
#include "GUID/GUID.h"


namespace Lumina
{
    
    /**
     *  Generic Asset Handle, prefer to use the template version.
     */
    class FAssetHandle
    {
    public:

        FAssetHandle() = default;
        FAssetHandle(const FAssetPath& InPath, EAssetType InType)
            : AssetPath(InPath)
            , AssetType(InType)
            , AssetRecord(nullptr)
        {
        }

        virtual ~FAssetHandle() = default;
        

        /** Checks if we have a valid AssetID, but doesn't signify of the asset is loaded */
        FORCEINLINE bool IsSet() const { return AssetPath.IsValid(); }
        
        
        FORCEINLINE const FAssetPath& GetAssetPath() const { return AssetPath; }

        FORCEINLINE EAssetType GetAssetType() const { return AssetType; }
        
        FORCEINLINE bool HasRecord() const { return AssetRecord != nullptr; }

        FORCEINLINE EAssetLoadState GetLoadState() const { return HasRecord() ? AssetRecord->GetLoadState() : EAssetLoadState::Unloaded; }
        FORCEINLINE bool IsLoaded() const { return GetLoadState() == EAssetLoadState::Loaded; }
        FORCEINLINE bool IsUnLoaded() const { return GetLoadState() == EAssetLoadState::Unloaded; }
        FORCEINLINE bool IsLoading() const { return GetLoadState() == EAssetLoadState::Unloading; }


        FORCEINLINE bool operator==(const FAssetHandle& Other) const { return AssetPath == Other.AssetPath; }
        FORCEINLINE bool operator!=(const FAssetHandle& Other) const { return AssetPath != Other.AssetPath; }

        
        friend FArchive& operator << (FArchive& Ar, FAssetHandle& Data)
        {
            Ar << Data.AssetPath;
            Ar << Data.AssetType;
            
            return Ar;
        }


    public:

        /** Path to this asset, serialized. */
        FAssetPath                      AssetPath;

        /** Type this asset is, serailized. */
        EAssetType                      AssetType;

        /** Transient record containing information about this asset. */
        TRefCountPtr<FAssetRecord>      AssetRecord;
        
    };

    
    template <typename T>
    class TAssetHandle : public FAssetHandle
    {
    public:
        
        TAssetHandle() = default;

        FORCEINLINE bool operator == (nullptr_t) const { return AssetRecord == nullptr; }
        FORCEINLINE bool operator != (nullptr_t) const { return AssetRecord != nullptr; }
        //FORCEINLINE bool operator == (const FAssetHandle& Other) const { return AssetPath == Other.AssetPath; }
        //FORCEINLINE bool operator != (const FAssetHandle& Other) const { return AssetPath != Other.AssetPath; }

        FORCEINLINE const T* GetPtr() const { Assert(IsLoaded()); return (const T*)AssetRecord->GetConstAssetPtr(); }
        FORCEINLINE const T* operator ->() const { return GetPtr(); }
        
        
        template<typename U>
        FORCEINLINE bool operator == (const TAssetHandle<T>& Other) { return AssetPath = Other.AssetPath; }

        template<typename U>
        FORCEINLINE bool operator != (const TAssetHandle<T>& Other) { return AssetPath != Other.AssetPath; }
        

        friend FArchive& operator << (FArchive& Ar, TAssetHandle& Data)
        {
            Ar << Data.AssetPath;
            Ar << Data.AssetType;

            return Ar;
        }
        
    };
}

namespace eastl
{
    template <typename T>
    struct hash<Lumina::TAssetHandle<T>>
    {
        std::size_t operator()(const Lumina::TAssetHandle<T>& handle) const noexcept
        {
            return eastl::hash<Lumina::FAssetHandle>()(handle.AssetHandle);
        }
    };
}

namespace eastl
{
    template <>
    struct hash<Lumina::FAssetHandle>
    {
        size_t operator()(const Lumina::FAssetHandle& Handle) const noexcept
        {
            return eastl::hash<Lumina::FAssetPath>{}(Handle.GetAssetPath());
        }
    };
}