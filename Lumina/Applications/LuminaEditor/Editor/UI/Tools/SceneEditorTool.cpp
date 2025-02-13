#include "SceneEditorTool.h"

#include "Scene/Entity/Components/NameComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"


namespace Lumina
{
    FSceneEditorTool::FSceneEditorTool(const IEditorToolContext* Context, FScene* InScene)
        : FEditorTool(Context, "Scene Editor", InScene), OutlinerContext()
    {
        Assert(Scene != nullptr);
    }

    void FSceneEditorTool::OnInitialize(const FUpdateContext& UpdateContext)
    {
        CreateToolWindow("Outliner", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawOutliner( Context, bisFocused );
        });
        
        CreateToolWindow("Entity Details", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawEntityEditor( Context, bisFocused );
        });

        OutlinerContext.DrawItemContextMenuFunction = [](const TVector<FTreeListViewItem*> Items)
        {
            
        };
    }

    void FSceneEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID topDockID = 0, bottomLeftDockID = 0, bottomCenterDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.5f, &bottomCenterDockID, &topDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.66f, &bottomCenterDockID, &bottomLeftDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.5f, &bottomRightDockID, &bottomCenterDockID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), topDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Outliner").c_str(), bottomLeftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Entity Details").c_str(), bottomCenterDockID);

    }

    void FSceneEditorTool::DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.65f, 0.15f, 1.0f));
        if (ImGui::Button("Create New Entity", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            CreateEntity();
        }
        ImGui::PopStyleColor();
        
        OutlinerListView.Draw(OutlinerContext);
        
    }

    void FSceneEditorTool::DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::CreateEntity()
    {
        Entity NewEntity = Scene->CreateEntity(FTransform(), FName("New Entity"));
        OutlinerListView.AddItemToTree<FEntityListViewItem>(nullptr, eastl::move(NewEntity));
    }
}
