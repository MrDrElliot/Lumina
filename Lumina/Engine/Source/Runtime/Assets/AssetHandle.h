#pragma once

#include "AssetTypes.h"
#include "AssetHeader.h"
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
        FAssetHandle(const FAssetPath& InPath) :AssetPath(InPath) {}
        virtual ~FAssetHandle() = default;
        

        /** Checks if we have a valid AssetID, but doesn't signify of the asset is loaded */
        FORCEINLINE bool IsSet() const { return AssetPath.IsValid(); }

        FORCEINLINE void Clear() { Assert(AssetPtr == nullptr); AssetPath = {}; }

        FORCEINLINE bool IsLoaded() const { return AssetPtr != nullptr; }
        
        FORCEINLINE const FAssetPath& GetAssetPath() const { return AssetPath; }

        FORCEINLINE EAssetType GetAssetType() const { return AssetType; }

        FORCEINLINE uint32 GetReferenceCount() const { return AssetPtr.use_count(); }

        FORCEINLINE bool operator==(const FAssetHandle& Other) const { return AssetPath == Other.AssetPath; }
        FORCEINLINE bool operator!=(const FAssetHandle& Other) const { return AssetPath != Other.AssetPath; }

        
        
        friend FArchive& operator << (FArchive& Ar, FAssetHandle& Data)
        {
            Ar << Data.AssetPath;
            Ar << Data.AssetType;
            
            return Ar;
        }


    public:

        FAssetPath          AssetPath;
        EAssetType          AssetType = EAssetType::Max;
        TSharedPtr<IAsset>  AssetPtr;
        
    };

    
    template <typename T>
    class TAssetHandle : public FAssetHandle
    {
    public:

        //static_assert(std::is_base_of_v<IAsset, T>, "T must be derrived from IAsset");
        
        TAssetHandle() = default;
        TAssetHandle(const FAssetPath& InPath) : FAssetHandle(InPath) { Assert(InPath.IsValid()); }
        
        FORCEINLINE bool operator == (nullptr_t) const { return AssetPtr == nullptr; }
        FORCEINLINE bool operator != (nullptr_t) const { return AssetPtr != nullptr; }
        //FORCEINLINE bool operator == (const FAssetHandle& Other) const { return AssetPath == Other.AssetPath; }
        //FORCEINLINE bool operator != (const FAssetHandle& Other) const { return AssetPath != Other.AssetPath; }

        FORCEINLINE TSharedPtr<T> GetPtr() const { Assert(AssetPtr != nullptr); return eastl::dynamic_shared_pointer_cast<T>(AssetPtr); }

        FORCEINLINE TSharedPtr<T> operator ->()  { Assert(AssetPtr != nullptr); return GetPtr(); }

        
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