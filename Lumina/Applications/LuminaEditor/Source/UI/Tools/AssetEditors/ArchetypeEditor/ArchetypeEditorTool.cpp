#include "ArchetypeEditorTool.h"


namespace Lumina
{
    static const char* ArchetypePropertiesName        = "ArchetypeProperties";

    FArchetypeEditorTool::FArchetypeEditorTool(IEditorToolContext* Context, CObject* InAsset)
        :FAssetEditorTool(Context, InAsset->GetName().ToString(), InAsset)
    {
    }

    void FArchetypeEditorTool::OnInitialize()
    {
        CreateToolWindow(ArchetypePropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
           ImGui::TextUnformatted("Thingy"); 
        });
    }

    void FArchetypeEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FArchetypeEditorTool::OnAssetLoadFinished()
    {
        FAssetEditorTool::OnAssetLoadFinished();
    }

    void FArchetypeEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        FAssetEditorTool::DrawToolMenu(UpdateContext);
    }

    void FArchetypeEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(ArchetypePropertiesName).c_str(), rightDockID);
    }
}
