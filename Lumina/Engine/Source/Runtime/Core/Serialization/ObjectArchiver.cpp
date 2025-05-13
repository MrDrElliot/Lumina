#include "ObjectArchiver.h"

#include "Core/Object/Object.h"
#include "Core/Object/ObjectCore.h"

namespace Lumina
{
    FArchive& FObjectArchiver::operator<<(CObject*& Obj)
    {
        if (IsWriting())
        {
            FString LoadedString;
            InnerArchive << LoadedString;

            if (LoadedString.empty())
            {
                Obj = nullptr;
                return *this;
            }

            FWString WString = UTF8_TO_WIDE(LoadedString);
            Obj = FindObject<CObject>(WString.c_str());

            if (Obj && bLoadIfFindFails)
            {
                Obj = LoadObject<CObject>(WString.c_str());
            }
            
        }
        else if (Obj)
        {
            FString SavedString(Obj->GetPathName());
            InnerArchive << SavedString;
        }
        else
        {
            FString EmptyString;
            InnerArchive << EmptyString;
        }

        return *this;
    }
}
