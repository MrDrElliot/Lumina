#pragma once
#include <memory>

#include "Assets/AssetTypes.h"
#include "Core/Singleton/Singleton.h"
#include "Factory.h"

namespace Lumina
{
    class FFactory;

    class FFactoryRegistry : public TSingleton<FFactoryRegistry>
    {
    public:
        using FactoryPtr = std::unique_ptr<FFactory>;

        FFactoryRegistry();
        
        void Shutdown() override {}

        template<typename T, EAssetType AssetType, typename = std::enable_if_t<std::is_base_of_v<FFactory, T>>>
        void RegisterFactory()
        {
            FactoryMap[AssetType] = std::make_unique<T>();
        }

        FFactory* GetFactory(EAssetType AssetType)
        {
            auto it = FactoryMap.find(AssetType);
            return it != FactoryMap.end() ? it->second.get() : nullptr;
        }

    private:
        
        eastl::unordered_map<EAssetType, FactoryPtr> FactoryMap;
    };
}
