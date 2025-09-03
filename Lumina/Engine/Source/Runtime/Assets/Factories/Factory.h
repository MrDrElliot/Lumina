#pragma once

#include "Core/Object/ObjectMacros.h"
#include "Core/Object/Object.h"
#include "Factory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CFactory : public CObject
    {
        GENERATED_BODY()

    public:
        
        virtual FString GetAssetName() const { return ""; }
        
        CObject* TryCreateNew(const FString& Path);
        
        virtual CObject* CreateNew(const FName& Name, CPackage* Package) { return nullptr; }
        
        virtual FString GetDefaultAssetCreationName(const FString& InPath) { return "New_Asset"; }

        virtual void TryImport(const FString& RawPath, const FString& DestinationPath) { }

        virtual CClass* GetSupportedType() const { return nullptr; }

        static bool ShowImportDialogue(CFactory* Factory, const FString& RawPath, const FString& DestinationPath);
        static bool ShowCreationDialogue(CFactory* Factory, const FString& Path);

        virtual bool HasImportDialogue() const { return false; }
        virtual bool HasCreationDialogue() const { return false; }
    protected:
        
        virtual bool DrawImportDialogue(const FString& RawPath, const FString& DestinationPath, bool& bShouldClose) { return true; }
        virtual bool DrawCreationDialogue(const FString& Path, bool& bShouldClose) { return true; }
        
    };
}
