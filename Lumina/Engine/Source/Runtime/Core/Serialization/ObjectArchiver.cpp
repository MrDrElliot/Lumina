#include "ObjectArchiver.h"

#include "Core/Object/Object.h"
#include "Core/Object/ObjectCore.h"

namespace Lumina
{
    FArchive& FObjectProxyArchiver::operator<<(CObject*& Obj)
    {
        return *this;
    }

    FArchive& FObjectProxyArchiver::operator<<(FObjectHandle& Value)
    {
        if (IsWriting())
        {
            if (Value)
            {
                FString SavedString(Value.Resolve()->GetQualifiedName().c_str());
                InnerArchive << SavedString;
            }
            else
            {
                FString EmptyString;
                InnerArchive << EmptyString;
            }
        }
        else if (IsReading())
        {
            FString LoadedString;
            InnerArchive << LoadedString;

            if (LoadedString.empty())
            {
                Value = nullptr;
                return *this;
            }

            Value = FindObject<CObject>(nullptr, LoadedString);

            if (!Value && bLoadIfFindFails)
            {
                Value = LoadObject<CObject>(nullptr, LoadedString);
            }   
        }

        return *this;
    }
}
