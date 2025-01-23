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
            : AssetPtr(nullptr), AssetHandle(InHandle)
        {
        }
        
        TAssetHandle(const FAssetMetadata& Metadata)
            : AssetPtr(nullptr), AssetHandle(Metadata.Guid)
        {
        }

        // Custom copy constructor
        TAssetHandle(const TAssetHandle<AssetType>& other)
            : AssetPtr(nullptr), AssetHandle(other.AssetHandle)
        {
            if (other.AssetPtr) 
            {
                AssetPtr = other.AssetPtr;  // Copy the shared_ptr, which shares ownership
            }
        }

        // Main function to get the asset. Will only load it if not already loaded.
        std::shared_ptr<AssetType> Get()
        {
            if (!AssetPtr) 
            {
                AssetPtr = LoadAsset();
            }

            return AssetPtr;
        }

        // Const version of Get to avoid modification
        std::shared_ptr<AssetType> Get() const
        {
            if (!AssetPtr)
            {
                return LoadAsset();
            }

            return AssetPtr;
        }

        bool IsValid() const { return Get() != nullptr; }

        std::shared_ptr<AssetType> operator -> () { return Get(); }
        const std::shared_ptr<AssetType> operator -> () const { return Get(); }

        bool operator == (const TAssetHandle<AssetType>& Other) const 
        {
            return AssetPtr == Other.AssetPtr;  // Compare the internal AssetPtr of both handles
        }

        // Assignment operator to copy handle and asset pointer
        TAssetHandle<AssetType>& operator=(const TAssetHandle<AssetType>& other)
        {
            if (this != &other)
            {
                AssetHandle = other.AssetHandle;
                AssetPtr = other.AssetPtr;  // Copy the shared pointer (automatically handles reference counting)
            }
            return *this;
        }

        // Move assignment operator for better performance
        TAssetHandle<AssetType>& operator=(TAssetHandle<AssetType>&& other) noexcept
        {
            if (this != &other)
            {
                AssetHandle = std::move(other.AssetHandle);
                AssetPtr = std::move(other.AssetPtr);  // Move ownership of the shared pointer
            }
            return *this;
        }

        operator bool() const { return Get() != nullptr; }
        operator std::shared_ptr<AssetType>() { return Get(); }

    private:
        
        std::shared_ptr<AssetType> LoadAsset() const
        {
            AssetManager* Manager = AssetManager::Get();
            std::shared_ptr<LAsset> rawAsset = Manager->LoadSynchronous(AssetHandle);
            
            if (rawAsset)
            {
                return std::dynamic_pointer_cast<AssetType>(rawAsset);
            }

            return nullptr;
        }


        FAssetHandle AssetHandle;
        mutable std::shared_ptr<AssetType> AssetPtr = nullptr; // Mark as mutable to allow modification in const functions
    };
}
