#include "AssetManager.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/Factories/FactoryRegistry.h"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"

namespace Lumina
{
    AssetManager::AssetManager()
    {
    }

    AssetManager::~AssetManager()
    {
    }

    std::shared_ptr<LAsset> AssetManager::LoadSynchronous(const FAssetHandle& InHandle)
    {
        auto it = mAssetMap.find(InHandle);
        if (it != mAssetMap.end())
        {
            if (std::shared_ptr<LAsset> Asset = it->second.lock())
            {
                return Asset;
            }
        }
    
        AssetRegistry* Registry = AssetRegistry::Get();
        if (Registry->Exists(InHandle))
        {
            FAssetMetadata Metadata = Registry->GetMetadata(InHandle);
            FArchive Ar(EArchiverFlags::Reading);
            Ar.ReadFromFile(Metadata.Path);
        
            FFactoryRegistry* factoryRegistry = FFactoryRegistry::Get();
            FFactory* Factory = factoryRegistry->GetFactory(Metadata.AssetType);

            if (Factory)
            {
                std::shared_ptr<LAsset> NewAsset = Factory->CreateNew(Metadata, Ar);
                if (NewAsset)
                {
                    mAssetMap[InHandle] = NewAsset;
                    return NewAsset;
                }
            }
        }

        return nullptr;
    }


    void AssetManager::GetAliveAssets(TFastVector<std::shared_ptr<LAsset>>& OutAliveAssets)
    {
        TFastVector<FAssetHandle> DeadAssets;
        for (auto& KVP : mAssetMap)
        {
            if(KVP.second.lock())
            {
                OutAliveAssets.push_back(KVP.second.lock());
            }
            else
            {
                DeadAssets.push_back(KVP.first);
            }
        }

        for(auto& dead : DeadAssets)
        {
            mAssetMap.erase(dead);
        }
    }
}

