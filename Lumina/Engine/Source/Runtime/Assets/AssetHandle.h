#pragma once

#include "AssetTypes.h"
#include "AssetHeader.h"
#include "AssetRecord.h"
#include "Core/Application/Application.h"
#include "GUID/GUID.h"

namespace Lumina
{
    class FAssetRecord;

    
    /**
     *  Generic Asset Handle
     */
    class FAssetHandle
    {
    public:
        
        FAssetHandle() = default;
        FAssetHandle(const FGuid& InGUID) :Guid(InGUID) {}
        

        /** Checks if we have a valid AssetID, but doesn't signify of the asset is loaded */
        FORCEINLINE bool IsSet() const { return Guid.IsValid(); }

        FORCEINLINE void Clear() { Assert(Record == nullptr); Guid = {}; }

        FORCEINLINE const FGuid& GetAssetGuid() const { return Guid; }

        FORCEINLINE EAssetType GetAssetType() const { return AssetType; }
        
        FORCEINLINE FAssetRecord* GetRecord() const { return Record; }
        FORCEINLINE void SetRecord(FAssetRecord* InRecord) { Assert(Record == nullptr); Record = InRecord; }

        FORCEINLINE EAssetLoadState GetLoadState() const { return Record ? Record->GetLoadState() : EAssetLoadState::Unloaded; }
        FORCEINLINE bool IsLoading() const          { return GetLoadState() == EAssetLoadState::Loading;    }
        FORCEINLINE bool IsLoaded() const           { return GetLoadState() == EAssetLoadState::Loaded;     }
        FORCEINLINE bool IsUnloading() const        { return GetLoadState() == EAssetLoadState::Unloading;  }
        FORCEINLINE bool IsUnloaded() const         { return GetLoadState() == EAssetLoadState::Unloaded;   }
        FORCEINLINE bool HasLoadingFailed() const   { return GetLoadState() == EAssetLoadState::Failed;     }


        FORCEINLINE bool operator==(const FAssetHandle& Other) const { return Guid == Other.Guid; }
        FORCEINLINE bool operator!=(const FAssetHandle& Other) const { return Guid != Other.Guid; }

        
        FORCEINLINE FAssetHandle& operator=(const FAssetHandle& Other) 
        {
            if (this != &Other)
            {
                AssertMsg(Record == nullptr, "cannot change a loaded resource. Unload it first");
                Guid = Other.Guid;
                Record = Other.Record;
            }
    
            return *this;
        }

        FORCEINLINE FAssetHandle& operator=(FAssetHandle&& Other) noexcept
        {
            if (this != &Other)
            {
                Guid = Other.Guid;
                Record = Other.Record;

                Other.Guid = {};
                Other.Record = nullptr;
            }

            return *this;
        }
        
        friend FArchive& operator << (FArchive& Ar, FAssetHandle& Data)
        {
            Ar << Data.Guid;
            
            return Ar;
        }


    public:

        FGuid           Guid;
        EAssetType      AssetType = EAssetType::Max;
        FAssetRecord*   Record =    nullptr;
    };

    
    template <typename T>
    class TAssetHandle : public FAssetHandle
    {
    public:

        //static_assert(std::is_base_of_v<IAsset, T>, "T must be derrived from IAsset");
        
        TAssetHandle() = default;
        TAssetHandle(FGuid InGuid) : FAssetHandle(InGuid) { Assert(InGuid.IsValid()); }
        TAssetHandle(const FAssetHandle& Other) { operator=(Other); }
        
        FORCEINLINE bool operator == (nullptr_t) const { return Record == nullptr; }
        FORCEINLINE bool operator != (nullptr_t) const { return Record != nullptr; }
        FORCEINLINE bool operator == (const FAssetHandle& Other) const { return Guid == Other.Guid; }
        FORCEINLINE bool operator != (const FAssetHandle& Other) const { return Guid != Other.Guid; }

        FORCEINLINE const T* operator ->() { Assert(Record != nullptr); return reinterpret_cast<const T*>(Record->GetAssetData()); }
        FORCEINLINE const T* GetPtr() const { Assert(Record != nullptr); return reinterpret_cast<const T*>(Record->GetAssetData()); }

        FORCEINLINE TAssetHandle<T>& operator = (const FAssetHandle& Other)
        {
            AssertMsg(Record == nullptr || Record->IsUnloaded(), "Cannot change a loaded resource");

            if (Other.IsSet())
            {
                if (Other.GetAssetType() == T::StaticGetAssetType())
                {
                    FAssetHandle::operator=(Other);
                }
                else
                {
                    AssertMsg(0, "Tried to assign invalid asset type");
                }
            }
            else
            {
                Guid = {};
            }

            return *this;

        }

        template<typename U>
        FORCEINLINE bool operator == (const TAssetHandle<T>& Other) { return Guid = Other.Guid; }

        template<typename U>
        FORCEINLINE bool operator != (const TAssetHandle<T>& Other) { return Guid != Other.Guid; }
        
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
            return eastl::hash<Lumina::FGuid>{}(Handle.GetAssetGuid());
        }
    };
}