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
            FactoryMap[AssetType] = FMemory::New<T>();
            Factories.push_back(FactoryMap[AssetType]);
        }

        FFactory* GetFactory(EAssetType AssetType)
        {
            auto it = FactoryMap.find(AssetType);
            return it != FactoryMap.end() ? it->second : nullptr;
        }

        const TVector<FFactory*>& GetFactories() const { return Factories; }
        

    private:
        
        THashMap<EAssetType, FFactory*>     FactoryMap;
        TVector<FFactory*>                  Factories;
    };
}
