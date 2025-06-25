#pragma once
#include "Core/Object/Class.h"
#include "Core/Object/Object.h"
#include "AssetDefinition.generated.h"


namespace Lumina
{
    class CFactory;
}

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CAssetDefinition : public CObject
    {
        GENERATED_BODY()
    public:
        
        void PostCreateCDO() override;
        
        virtual FString GetAssetDisplayName() const { return ""; }

        virtual FString GetAssetDescription() const { return ""; }
        
        virtual CClass* GetAssetClass() const { return nullptr; }

        virtual CFactory* GetFactory() const { return nullptr; }

        virtual bool CanImport() { return false; }

        virtual FString GetImportFileExtension() { return ""; }
    };

    
    LUM_CLASS()
    class LUMINA_API CAssetDefinitionRegistry : public CObject
    {
        GENERATED_BODY()
    public:

        static CAssetDefinitionRegistry* Get();
        
        void RegisterDefinition(CAssetDefinition* InDef);

        void GetAssetDefinitions(TVector<CAssetDefinition*>& Definitions);
        
    private:

        THashMap<CClass*, CAssetDefinition*> AssetDefinitions;
        
        static CAssetDefinitionRegistry* Singleton;
    };
}
