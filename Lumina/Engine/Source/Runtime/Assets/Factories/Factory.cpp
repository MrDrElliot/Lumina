#include "Factory.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"

namespace Lumina
{
    CObject* CFactory::TryCreateNew(const FString& Path)
    {
        CPackage* Package = CPackage::CreatePackage(GetSupportedType()->GetName().ToString(), Path);
        FString FileName = Paths::FileName(Path);

        CObject* New = CreateNew(FileName.c_str(), Package);
        CPackage::SavePackage(Package, nullptr, Path.c_str());

        return New;
    }

}
