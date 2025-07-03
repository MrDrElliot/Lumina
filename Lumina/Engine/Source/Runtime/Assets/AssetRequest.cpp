
#include "AssetRequest.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    bool FAssetRequest::Process()
    {
        FString FullPath = Paths::ResolveVirtualPath(AssetPath);
        FString Name = Paths::FileName(AssetPath);

        CPackage* Package = CPackage::LoadPackage(FullPath.c_str());
        FName QualifiedName = MakeFullyQualifiedObjectName(Package, Name.c_str());
        PendingObject = FindObject<CObject>(QualifiedName);

        Package->LoadObject(PendingObject);
        
        return true;
    }
}
 