#include "AssetManager.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/Factories/FactoryRegistry.h"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"

namespace Lumina
{
    AssetManager::AssetManager()
    {
        FactoryRegistry* registry = FactoryRegistry::Get();
        registry->RegisterFactory(EAssetType::Texture, std::make_unique<FTextureFactory>());
        registry->RegisterFactory(EAssetType::StaticMesh, std::make_unique<FStaticMeshFactory>());
    }

    AssetManager::~AssetManager()
    {
    }

    std::shared_ptr<LAsset> AssetManager::LoadSynchronous(const FAssetHandle& InHandle)
    {

        if (mAssetMap.find(InHandle) != mAssetMap.end())
        {
            
        }
        
        AssetRegistry* Registry = AssetRegistry::Get();
        if(Registry->Exists(InHandle))
        {
            FAssetMetadata Metadata = Registry->GetMetadata(InHandle);
            FArchive Ar(EArchiverFlags::Reading);
            Ar.ReadFromFile(Metadata.Path);
            
            // Get the factory for the asset type
            FactoryRegistry* factoryRegistry = FactoryRegistry::Get();
            FFactory* Factory = factoryRegistry->GetFactory(Metadata.AssetType);

            if (Factory)
            {
                std::shared_ptr<LAsset> NewAsset = Factory->CreateNew(Metadata, Ar);
                mAssetMap[InHandle] = NewAsset;
                return NewAsset;
            }
        }
        return std::shared_ptr<LAsset>();
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

