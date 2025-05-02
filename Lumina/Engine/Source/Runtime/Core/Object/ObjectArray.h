#pragma once
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CObjectBase;

    struct FCObjectArray
    {
    public:
        FCObjectArray()
            : Objects(nullptr)
            , NumEntries(0)
            , MaxEntries(0)
        {}

        ~FCObjectArray()
        {
            delete[] Objects;
        }

        void Reserve(int32 NewMax)
        {
            if (NewMax <= MaxEntries)
                return;

            CObjectBase** NewObjects = new CObjectBase*[NewMax];

            // Copy existing
            for (int32 i = 0; i < NumEntries; ++i)
            {
                NewObjects[i] = Objects[i];
            }

            // Zero out the rest
            for (int32 i = NumEntries; i < NewMax; ++i)
            {
                NewObjects[i] = nullptr;
            }

            delete[] Objects;
            Objects = NewObjects;
            MaxEntries = NewMax;
        }

        /** Adds an object and returns its index. */
        int32 Add(CObjectBase* Object)
        {
            if (NumEntries >= MaxEntries)
            {
                Reserve(MaxEntries > 0 ? MaxEntries * 2 : 16);
            }

            int32 Index = NumEntries;
            Objects[Index] = Object;
            ++NumEntries;
            return Index;
        }

        CObjectBase* GetAt(int32 Index) const
        {
            if (Index < 0 || Index >= NumEntries)
                return nullptr;
            return Objects[Index];
        }

        int32 Num() const { return NumEntries; }

    private:
        CObjectBase** Objects;  // Single flat array
        int32 NumEntries;       // How many are used
        int32 MaxEntries;       // How many are allocated
    };

}
