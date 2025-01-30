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

    TSharedPtr<LAsset> AssetManager::LoadSynchronous(const FAssetHandle& InHandle)
    {
        auto it = mAssetMap.find(InHandle);
        if (it != mAssetMap.end())
        {
            if (TSharedPtr<LAsset> Asset = it->second.lock())
            {
                return Asset;
            }
        }
    
        AssetRegistry* Registry = AssetRegistry::Get();
        if (Registry->Exists(InHandle))
        {
            FAssetMetadata Metadata = Registry->GetMetadata(InHandle);

            FFactoryRegistry* factoryRegistry = FFactoryRegistry::Get();
            FFactory* Factory = factoryRegistry->GetFactory(Metadata.AssetType);

            if (Factory)
            {
                TVector<uint8> Buffer;
                if (!FFileHelper::LoadFileToArray(Buffer, Metadata.Path.c_str()) || Buffer.empty()) 
                {
                    LOG_ERROR("Failed to load asset from path: {0}", Metadata.Path);
                    return nullptr;
                }
                
                
                FMemoryReader Reader(Buffer);
                TSharedPtr<LAsset> NewAsset = Factory->CreateNew(Metadata, Reader);
                if (NewAsset)
                {
                    mAssetMap[InHandle] = NewAsset;
                    return NewAsset;
                }
            }
        }

        return nullptr;
    }


    void AssetManager::GetAliveAssets(TVector<TSharedPtr<LAsset>>& OutAliveAssets)
    {
        TVector<FAssetHandle> DeadAssets;
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

