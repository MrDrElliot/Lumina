#pragma once

#include "AssetTypes.h"
#include "AssetManager/AssetManager.h"
#include "Assets/Asset.h"


namespace Lumina
{
    class AssetRegistry;
}

namespace Lumina
{
    template <typename AssetType>
    class TAssetHandle
    {
    public:

        TAssetHandle()
        : AssetHandle()
        {
        }

        TAssetHandle(const FAssetHandle& InHandle)
            :AssetHandle(InHandle)
        {
        }
        
        TAssetHandle(const FAssetMetadata& Metadata)
            :AssetHandle(Metadata.Guid)
        {
        }

        std::shared_ptr<AssetType> Get()
        {
            if(AssetPtr)
            {
                return std::static_pointer_cast<AssetType>(AssetPtr);
            }
            else
            {
                AssetManager* Manager = AssetManager::Get();
                AssetPtr = std::static_pointer_cast<AssetType>(Manager->LoadSynchronous(AssetHandle));

                return std::static_pointer_cast<AssetType>(AssetPtr);
            }
       }

        const std::shared_ptr<AssetType>& Get() const
        {
            return std::static_pointer_cast<AssetType>(AssetPtr);
        }
        
        
        std::shared_ptr<AssetType> operator -> () { return Get(); }
        const std::shared_ptr<AssetType> operator -> () const { return Get(); }
        
        bool operator == (const TAssetHandle<AssetType>& Other) { return Get() == Get(); }
        operator bool() { return Get() != nullptr; }
        operator const bool() { return Get() != nullptr; }
        operator std::shared_ptr<AssetType>() { return Get();  }
        
    private:

        FAssetHandle AssetHandle;
        std::shared_ptr<LAsset> AssetPtr;
    };
    
}
