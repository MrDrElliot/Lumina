#include "SceneEditorTool.h"


namespace Lumina
{
    FSceneEditorTool::FSceneEditorTool(const IEditorToolContext* Context, const TRefPtr<FScene>& InScene)
        :FEditorTool(Context, "Scene Editor", InScene)
    {
        Assert(Scene != nullptr);
    }

    void FSceneEditorTool::OnInitialize(const FUpdateContext& UpdateContext)
    {
        CreateToolWindow("Outliner", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawOutliner( Context, bisFocused );
        });
        
        CreateToolWindow("Entity", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawEntityEditor( Context, bisFocused );
        });
        
        CreateToolWindow("Properties", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawPropertyEditor(Context, bisFocused);
        });
    }

    void FSceneEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID topDockID = 0, bottomLeftDockID = 0, bottomCenterDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.5f, &bottomCenterDockID, &topDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.66f, &bottomCenterDockID, &bottomLeftDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.5f, &bottomRightDockID, &bottomCenterDockID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), topDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Outliner").c_str(), bottomLeftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Entity").c_str(), bottomCenterDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Properties").c_str(), bottomRightDockID);

    }

    void FSceneEditorTool::DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }
}
