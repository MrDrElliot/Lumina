#pragma once

#include "Core/Object/Object.h"
#include "Factory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CFactory : public CObject
    {
        GENERATED_BODY()

    public:
        
        virtual CObject* CreateNew(const FString& Path) { return nullptr; }
        
        virtual FString GetAssetName() const { return ""; }
        
        virtual void CreateAssetFile(const FString& Path) { }
        
    };
}
