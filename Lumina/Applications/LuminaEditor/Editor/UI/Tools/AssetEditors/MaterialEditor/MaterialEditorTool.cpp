#include "MaterialEditorTool.h"

#include "imnodes.h"


namespace Lumina
{
    const char* MaterialGraphName       = "Material Graph";
    const char* MaterialPropertiesName  = "Material Properties";
    const char* MaterialPreviewName     = "Material Preview";

    void FMaterialEditorTool::OnInitialize()
    {
        CreateToolWindow(MaterialGraphName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            ImNodes::BeginNodeEditor();

            DrawMaterialGraph(Cxt);

            ImNodes::EndNodeEditor();
        });

        CreateToolWindow(MaterialPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawMaterialProperties(Cxt);
        });

        CreateToolWindow(MaterialPreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawMaterialPreview(Cxt);
        });
    }


    void FMaterialEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialEditorTool::Update(const FUpdateContext& UpdateContext)
    {
        
    }

    void FMaterialEditorTool::DrawMaterialGraph(const FUpdateContext& UpdateContext)
    {
        
    }

    void FMaterialEditorTool::DrawMaterialProperties(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialEditorTool::DrawMaterialPreview(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomLeftDockID = 0;

        // Split horizontally: Left (Material Graph) and Right (Material Preview)
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        // Split the left dock to create a bottom panel for Material Properties
        ImGui::DockBuilderSplitNode(leftDockID, ImGuiDir_Down, 0.3f, &bottomLeftDockID, &leftDockID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialGraphName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialPropertiesName).c_str(), bottomLeftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialPreviewName).c_str(), rightDockID);
    }

}
