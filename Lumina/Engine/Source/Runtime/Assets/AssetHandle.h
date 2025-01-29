#pragma once

#include "AssetTypes.h"
#include "AssetManager/AssetManager.h"

namespace Lumina
{
    class AssetRegistry;
    
    template <typename AssetType>
    class TAssetHandle
    {
    public:
        TAssetHandle() = default;

        TAssetHandle(const FAssetHandle& InHandle)
            : AssetPtr(nullptr), AssetHandle(InHandle) {}
        
        TAssetHandle(const FAssetMetadata& Metadata)
            : AssetPtr(nullptr), AssetHandle(Metadata.Guid) {}
        
        TAssetHandle(const TAssetHandle<AssetType>& other)
            : AssetPtr(nullptr), AssetHandle(other.AssetHandle)
        {
            if (other.AssetPtr) 
            {
                AssetPtr = other.AssetPtr;
            }
        }

        FORCEINLINE TSharedPtr<AssetType> Get()
        {
            if (AssetPtr)
            {
                return AssetPtr;
            }

            return LoadSynchronous();
        }
        
        FORCEINLINE TSharedPtr<AssetType> Get() const
        {
            if (AssetPtr)
            {
                return AssetPtr;
            }

            return LoadSynchronous();
        }

        FORCEINLINE TSharedPtr<AssetType> GetIfValid() const
        {
            return AssetPtr;
        }

        TSharedPtr<AssetType> LoadSynchronous() const
        {
            AssetManager* Manager = AssetManager::Get();
            if (AssetPtr)
            {
                return std::dynamic_pointer_cast<AssetType>(AssetPtr);
            }

            AssetPtr = std::dynamic_pointer_cast<AssetType>(Manager->LoadSynchronous(AssetHandle));

            return AssetPtr;
        }
        

        TSharedPtr<AssetType> operator         -> () { return Get(); }
        const TSharedPtr<AssetType> operator   -> () const { return Get(); }

        bool operator == (const TAssetHandle<AssetType>& Other) const 
        {
            return AssetPtr == Other.AssetPtr;
        }

        TAssetHandle<AssetType>& operator=(const TAssetHandle<AssetType>& other)
        {
            if (this != &other)
            {
                AssetHandle = other.AssetHandle;
                AssetPtr = other.AssetPtr;
            }
            return *this;
        }

        TAssetHandle<AssetType>& operator=(TAssetHandle<AssetType>&& other) noexcept
        {
            if (this != &other)
            {
                AssetHandle =   std::move(other.AssetHandle);
                AssetPtr =      std::move(other.AssetPtr);
            }
            return *this;
        }

        bool IsValid() const { return Get() != nullptr; }
        operator bool() const { return Get() != nullptr; }
        operator TSharedPtr<AssetType>() { return Get(); }


    public:
        
        FAssetHandle AssetHandle;
        mutable TSharedPtr<AssetType> AssetPtr = nullptr;
    };
}

namespace std
{
    template <typename AssetType>
    struct hash<Lumina::TAssetHandle<AssetType>>
    {
        std::size_t operator()(const Lumina::TAssetHandle<AssetType>& handle) const noexcept
        {
            return std::hash<Lumina::FAssetHandle>()(handle.AssetHandle);
        }
    };
}
