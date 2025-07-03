#include "AssetEditorTool.h"
#include "Core/Object/Class.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Core/Object/Package/Package.h"

namespace Lumina
{
    void FAssetEditorTool::Deinitialize(const FUpdateContext& UpdateContext)
    {
        OnSave();
        FEditorTool::Deinitialize(UpdateContext);
    }

    void FAssetEditorTool::OnSave()
    {
        FString FullPath = Paths::ResolveVirtualPath(Asset->GetPathName());
        Paths::AddPackageExtension(FullPath);
        CPackage::SavePackage(Asset->GetPackage(), Asset, FullPath.c_str());
    }
}
