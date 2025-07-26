#include "AssetEditorTool.h"
#include "Core/Object/Class.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Core/Object/Package/Package.h"
#include "Tools/UI/ImGui/ImGuiX.h"

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
        bool bSuccess = CPackage::SavePackage(Asset->GetPackage(), Asset, FullPath.c_str());

        if (bSuccess)
        {
            ImGuiX::Notifications::NotifySuccess("Successfully saved package: \"%s\"", Asset->GetPathName().c_str());
        }
        else
        {
            ImGuiX::Notifications::NotifyError("Failed to save package: \"%s\"", Asset->GetPathName().c_str());
        }

    }
}
