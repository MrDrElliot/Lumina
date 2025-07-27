#include "PackageLoader.h"

#include "Core/Object/ObjectArray.h"
#include "Core/Object/Package/Package.h"

namespace Lumina
{
    FArchive& FPackageLoader::operator<<(CObject*& Value)
    {
        FObjectPackageIndex Index;
        FArchive& Ar = *this;
        Ar << Index;
        
        Value = Package->IndexToObject(Index);
        Package->LoadObject(Value);
        
        return Ar;
    }

    FArchive& FPackageLoader::operator<<(FObjectHandle& Value)
    {
        FObjectPackageIndex Index;
        FArchive& Ar = *this;
        Ar << Index;

        CObject* Object = Package->IndexToObject(Index);
        Value = GObjectArray.ToHandle(Object);
        Package->LoadObject(Object);
        
        return Ar;    }
}
