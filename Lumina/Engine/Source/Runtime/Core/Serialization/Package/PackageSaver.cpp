﻿#include "PackageSaver.h"

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

        if (Value->GetPackage() == nullptr)
        {
            return *this;
        }
        
        if (Value->GetPackage() == SaveContext->CurrentPackage)
        {
            SaveContext->AddExport(Value);
        }
        else if (Value->GetPackage())
        {
            SaveContext->AddImport(Value);
        }

        if (Value->GetPackage() == SaveContext->CurrentPackage)
        {
            Value->Serialize(*this);
        }
        
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
                if (ObjectToIndexMap.find(Value) != ObjectToIndexMap.end())
                {
                    Index = FObjectPackageIndex::FromImport(ObjectToIndexMap[Value]);
                }
                else
                {
                    Index = FObjectPackageIndex::FromImport(CurrentImportIndex);
                    ObjectToIndexMap.emplace(Value, CurrentImportIndex);
                    CurrentImportIndex++;
                }
            }
        }
        
        *this << Index;
        
        return *this;
    }
}
