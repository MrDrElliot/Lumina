#include "PackageLoader.h"

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
}
