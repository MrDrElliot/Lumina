#pragma once
#include "Core/Object/Object.h"
#include "Core/Object/Class.h"
#include "Core/Object/ObjectCore.h"
#include "Core/Serialization/MemoryArchiver.h"

namespace Lumina
{
    class FPackageLoader : public FMemoryReader
    {
    public:

        explicit FPackageLoader(const TVector<uint8>& InBytes, bool bIsPersistent = false)
            : FMemoryReader(InBytes, bIsPersistent)
        {
        }


        virtual FArchive& operator<<(CObject*& Value) override
        {
            FString LoadedString;
            FArchive::operator<<(LoadedString);

            if (LoadedString.empty())
            {
                Value = nullptr;
                return *this;
            }


            FWString WString = UTF8_TO_WIDE(LoadedString);
            CClass* Class = FindObject<CClass>(WString.c_str());

            if (Class)
            {
                Value = NewObject(Class);
            }
            
            return *this;
        }
        
    };
}
