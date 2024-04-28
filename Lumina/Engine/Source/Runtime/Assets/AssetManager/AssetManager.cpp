#include "AssetManager.h"

#include "Source/Runtime/Assets/AssetRegistry/AssetRegistry.h"

namespace Lumina
{
    std::shared_ptr<FAssetRegistry> FAssetManager::AssetRegistry = nullptr;
    
    FAssetManager::FAssetManager(const FApplicationSpecs& InAppSpecs)
    {
        AssetRegistry = std::make_shared<FAssetRegistry>();
    }

    FAssetManager::~FAssetManager()
    {
    }

    FAssetManager& FAssetManager::Get()
    {
        return *FApplication::GetAssetManager();
    }

    FAssetRegistry& FAssetManager::GetRegistry()
    {
        return *AssetRegistry;
    }
}
