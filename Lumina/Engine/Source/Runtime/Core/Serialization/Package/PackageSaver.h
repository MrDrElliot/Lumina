#pragma once
#include "Core/Object/Object.h"
#include "Core/Object/Class.h"
#include "Core/Serialization/MemoryArchiver.h"

namespace Lumina
{
    class FPackageSaver : public FMemoryWriter
    {
    public:

        FPackageSaver(TVector<uint8>& InBytes, bool bSetOffset = false)
            :FMemoryWriter(InBytes, bSetOffset)
        {}

        virtual FArchive& operator<<(CObject*& Value) override
        {
            FString Path = "nullptr";

            if (Value != nullptr)
            {
                Path = Value->GetClass()->GetName().c_str();
            }

            FArchive::operator<<(Path);


            return *this;
        }
    
    };
}
