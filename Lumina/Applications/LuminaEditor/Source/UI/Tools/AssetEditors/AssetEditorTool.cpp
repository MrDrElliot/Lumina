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
        FEditorTool::Deinitialize(UpdateContext);
    }

    FString FAssetEditorTool::GetToolName() const
    {
        return Asset->GetPathName();
    }

    void FAssetEditorTool::Update(const FUpdateContext& UpdateContext)
    {
        if (!bAssetLoadBroadcasted && Asset != nullptr)
        {
            OnAssetLoadFinished();
            bAssetLoadBroadcasted = true;
        }
    }

    void FAssetEditorTool::OnSave()
    {
        FString FullPath = Paths::ResolveVirtualPath(Asset->GetPathName());
        Paths::AddPackageExtension(FullPath);

        if (CPackage::SavePackage(Asset->GetPackage(), Asset, FullPath.c_str()))
        {
            ImGuiX::Notifications::NotifySuccess("Successfully saved package: \"%s\"", Asset->GetPathName().c_str());
        }
        else
        {
            ImGuiX::Notifications::NotifyError("Failed to save package: \"%s\"", Asset->GetPathName().c_str());
        }

    }
}
