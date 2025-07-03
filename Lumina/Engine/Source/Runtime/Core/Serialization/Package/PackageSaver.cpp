#include "PackageSaver.h"

#include "Core/Object/Package/Package.h"

namespace Lumina
{
    void FSaveContext::AddImport(CObject* Import)
    {
        if (!SeenImports.insert(Import).second)
        {
            return;
        }
        
        Imports.push_back(Import);
    }

    void FSaveContext::AddExport(CObject* Export)
    {
        if (!SeenExports.insert(Export).second)
        {
            return;
        }
        
        Exports.push_back(Export);
    }

    FArchive& FSaveReferenceBuilderArchive::operator<<(CObject*& Value)
    {
        if (Value == nullptr)
        {
            return *this;
        }

        if (Value->GetPackage() && Value->GetPackage() == SaveContext->CurrentPackage)
        {
            SaveContext->AddExport(Value);
        }
        else if (Value->GetPackage())
        {
            SaveContext->AddImport(Value);
        }
        
        Value->Serialize(*this);
        
        return *this;
    }

    //---------------------------------------------------------------------------------------------
    
    FArchive& FPackageSaver::operator<<(CObject*& Value)
    {
        FObjectPackageIndex Index;
        if (Value)
        {
            if (Value->GetPackage() == Package)
            {
                Index = FObjectPackageIndex(Value->GetLoaderIndex());
            }
            else
            {
                Index = FObjectPackageIndex::FromImport(Package->ImportIndex);
            }
        }
        
        *this << Index;
        
        return *this;
    }
}
