#include "AssetDefinition.h"

namespace Lumina
{
    CAssetDefinitionRegistry* CAssetDefinitionRegistry::Singleton = nullptr;

    void CAssetDefinition::PostCreateCDO()
    {
        CAssetDefinitionRegistry::Get()->RegisterDefinition(this);
    }

    CAssetDefinitionRegistry* CAssetDefinitionRegistry::Get()
    {
        if (Singleton == nullptr)
        {
            Singleton = NewObject<CAssetDefinitionRegistry>(TRANSIENT_PACKAGE, "AssetDefinitionRegistry_Singleton");
        }
         
        return Singleton;
    }

    void CAssetDefinitionRegistry::RegisterDefinition(CAssetDefinition* InDef)
    {
        if (CClass* Class = InDef->GetAssetClass())
        {
            Assert(AssetDefinitions.find(Class) == AssetDefinitions.end())
            AssetDefinitions.insert_or_assign(Class, InDef);
        }
    }

    void CAssetDefinitionRegistry::GetAssetDefinitions(TVector<CAssetDefinition*>& Definitions)
    {
        Definitions.reserve(AssetDefinitions.size());
        for (const auto& Def : AssetDefinitions)
        {
            Definitions.push_back(Def.second);
        }
    }
}
