#pragma once
#include <memory>

#include "Assets/AssetTypes.h"
#include "Core/Singleton/Singleton.h"
#include "Factory.h"

namespace Lumina
{
    class FFactory;

    class FactoryRegistry : public TSingleton<FactoryRegistry>
    {
    public:
        using FactoryPtr = std::unique_ptr<FFactory>;

        void Shutdown() override {}

        void RegisterFactory(EAssetType AssetType, FactoryPtr Factory)
        {
            FactoryMap[AssetType] = std::move(Factory);
        }

        FFactory* GetFactory(EAssetType AssetType)
        {
            auto it = FactoryMap.find(AssetType);
            return it != FactoryMap.end() ? it->second.get() : nullptr;
        }

    private:
        
        std::unordered_map<EAssetType, FactoryPtr> FactoryMap;
    };
}
