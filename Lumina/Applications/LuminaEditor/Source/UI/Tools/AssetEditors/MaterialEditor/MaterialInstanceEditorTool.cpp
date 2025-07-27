#include "MaterialInstanceEditorTool.h"

namespace Lumina
{
    static const char* MaterialEditorName          = "Material Editor";
    static const char* MaterialPreviewName         = "Material Preview";
    
    void FMaterialInstanceEditorTool::OnInitialize()
    {
        CreateToolWindow(MaterialEditorName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            ImGui::Text("Material Editor");
        });

        CreateToolWindow(MaterialPreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            ImGui::Text("Material Preview");
        });
    }

    void FMaterialInstanceEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialInstanceEditorTool::OnAssetLoadFinished()
    {
    }

    void FMaterialInstanceEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        
    }

    void FMaterialInstanceEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        // Split horizontally: Left (Material Graph) and Right (Material Preview)
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        // Create a full bottom dock by splitting the main dockspace (InDockspaceID) only once
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        // Dock the windows into their respective locations
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialPreviewName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialEditorName).c_str(), rightDockID);
    }
    
}
