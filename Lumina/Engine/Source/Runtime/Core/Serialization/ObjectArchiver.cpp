#include "ObjectArchiver.h"

#include "Core/Object/Object.h"
#include "Core/Object/ObjectCore.h"

namespace Lumina
{
    FArchive& FObjectProxyArchiver::operator<<(CObject*& Obj)
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

            Obj = FindObject<CObject>(nullptr, LoadedString);

            if (Obj && bLoadIfFindFails)
            {
                Obj = LoadObject<CObject>(nullptr, LoadedString);
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
