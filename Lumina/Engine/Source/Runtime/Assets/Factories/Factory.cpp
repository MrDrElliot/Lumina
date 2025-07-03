#include "Factory.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"

namespace Lumina
{
    CObject* CFactory::TryCreateNew(const FString& Path)
    {
        CPackage* Package = CPackage::CreatePackage(Path);
        FString FileName = Paths::FileName(Path);

        return CreateNew(FileName.c_str(), Package);
    }

}
