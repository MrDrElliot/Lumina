
#include "AssetRequest.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    bool FAssetRequest::Process()
    {
        FString FullPath = Paths::RemoveExtension(AssetPath);
        FString Name = Paths::FileName(FullPath);
        FullPath = Paths::ResolveVirtualPath(FullPath);

        CPackage* Package = CPackage::LoadPackage(FullPath.c_str());
        FName QualifiedName = MakeFullyQualifiedObjectName(Package, Name.c_str());
        PendingObject = FindObject<CObject>(QualifiedName);

        Package->LoadObject(PendingObject);
        
        return true;
    }
}
 