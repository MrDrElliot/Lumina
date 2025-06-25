#pragma once

#include "Core/Object/ObjectMacros.h"
#include "Core/Object/Object.h"
#include "Factory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CFactory : public CObject
    {
        GENERATED_BODY()

    public:
        
        virtual FString GetAssetName() const { return ""; }
        
        virtual void CreateAssetFile(const FString& Path) { }

        virtual FString GetDefaultAssetCreationName(const FString& InPath) { return "New_Asset"; }

        virtual void TryImport(const FString& RawPath, const FString& DestinationPath) { }
        
    };
}
