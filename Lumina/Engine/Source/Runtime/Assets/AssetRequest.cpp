
#include "AssetRequest.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    void FAssetRequest::WaitForTask()
    {
        FTaskSystem::Get().WaitForTask(Task);
    }

    bool FAssetRequest::Process()
    {
        FString FullPath = Paths::RemoveExtension(AssetPath);
        FString Name = Paths::GetExtension(AssetPath);
        FullPath = Paths::ResolveVirtualPath(FullPath);

        CPackage* Package = CPackage::LoadPackage(FullPath.c_str());
        if (Package == nullptr)
        {
            LOG_INFO("Failed to load package at path: {}", FullPath);
            return false;
        }
        
        FName QualifiedName = MakeFullyQualifiedObjectName(Package, Name.c_str());
        PendingObject = FindObject<CObject>(QualifiedName);

        Package->LoadObject(PendingObject);
        
        return true;
    }
}
 