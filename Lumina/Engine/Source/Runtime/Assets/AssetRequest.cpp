
#include "AssetRequest.h"

#include "Core/Object/Cast.h"
#include "Core/Object/ObjectRedirector.h"
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

        // The extension is just an easy way to get the string after the "." delimiter.
        FString Name = Paths::GetExtension(AssetPath);
        
        FullPath = Paths::ResolveVirtualPath(FullPath);

        CPackage* Package = CPackage::LoadPackage(FullPath.c_str());
        if (Package == nullptr)
        {
            LOG_INFO("Failed to load package at path: {}", FullPath);
            return false;
        }
        
        PendingObject = FindObject<CObject>(Package, Name);
        if (PendingObject != nullptr)
        {
            if (PendingObject->HasAnyFlag(OF_NeedsLoad))
            {
                Package->LoadObject(PendingObject);
            }
        
            if (PendingObject->HasAnyFlag(OF_NeedsPostLoad))
            {
                PendingObject->PostLoad();
                PendingObject->ClearFlags(OF_NeedsPostLoad);
            }

            if (CObjectRedirector* Redirector = Cast<CObjectRedirector>(PendingObject))
            {
                PendingObject = Redirector->RedirectionObject;
            }
        }
        
        return true;
    }
}
 