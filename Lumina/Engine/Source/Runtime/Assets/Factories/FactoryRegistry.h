#pragma once
#include <memory>

#include "Assets/AssetTypes.h"
#include "Factory.h"

namespace Lumina
{
    class FFactory;

    class FFactoryRegistry
    {
    public:
        
        FFactoryRegistry();
        ~FFactoryRegistry();
        
        template<typename T, EAssetType AssetType, typename = std::enable_if_t<std::is_base_of_v<FFactory, T>>>
        void RegisterFactory()
        {
            FactoryMap[AssetType] = new T();
        }

        FFactory* GetFactory(EAssetType AssetType)
        {
            auto it = FactoryMap.find(AssetType);
            return it != FactoryMap.end() ? it->second : nullptr;
        }

    private:
        
        THashMap<EAssetType, FFactory*> FactoryMap;
    };
}
