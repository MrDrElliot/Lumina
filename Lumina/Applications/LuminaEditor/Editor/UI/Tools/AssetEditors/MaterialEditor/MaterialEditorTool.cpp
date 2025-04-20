#include "MaterialEditorTool.h"
#include "imnodes/imnodes.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"
#include "UI/Tools/NodeGraph/Material/MaterialNodeGraph.h"


namespace Lumina
{
    const char* MaterialGraphName           = "Material Graph";
    const char* MaterialPropertiesName      = "Material Properties";
    const char* MaterialPreviewName         = "Material Preview";
    const char* MaterialCompileLogName      = "Compilation Log";

    void FMaterialEditorTool::OnInitialize()
    {
        CreateToolWindow(MaterialGraphName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawMaterialGraph(Cxt);
        });

        CreateToolWindow(MaterialPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawMaterialProperties(Cxt);
        });

        CreateToolWindow(MaterialPreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawMaterialPreview(Cxt);
        });

        CreateToolWindow(MaterialCompileLogName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawCompilationLog(Cxt);
        });

        NodeGraph = nullptr;//NewObject<CMaterialNodeGraph>();

    }


    void FMaterialEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        FMemory::Delete(NodeGraph);
    }

    void FMaterialEditorTool::OnAssetLoadFinished()
    {
        LOG_DEBUG("Asset Load Finished");

        TVector<uint8> Data = Asset->GetAssetPtr<AMaterial>()->GraphData;
        FMemoryReader Reader(Data);
        
        NodeGraph->Serialize(Reader);
    }


    void FMaterialEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::MenuItem(LE_ICON_RECEIPT_TEXT" Compile"))
        {
            CompilationResult = FCompilationResultInfo();
            
            FMaterialCompiler Compiler;
            NodeGraph->CompileGraph(&Compiler);

            if (Compiler.HasErrors())
            {
                for (const FMaterialCompiler::FError& Error : Compiler.GetErrors())
                {
                    CompilationResult.CompilationLog += "ERROR - [" + Error.ErrorName + "]: " + Error.ErrorDescription + "\n";
                }
                
                CompilationResult.bIsError = true;
            }
            else
            {
                
                FString Tree = Compiler.BuildTree();
                CompilationResult.CompilationLog = "Material Compiled Successfully! Generated GLSL: \n \n \n" + Tree;
                CompilationResult.bIsError = false;

                AMaterial* Material = Asset->GetAssetPtr<AMaterial>();
                
                TVector<uint8> GraphData;
                FMemoryWriter Writer(GraphData);
                NodeGraph->Serialize(Writer);
                
                Material->GraphData = eastl::move(GraphData);
                Material->Save();
                
            }
        }
    }

    void FMaterialEditorTool::DrawMaterialGraph(const FUpdateContext& UpdateContext)
    {
        NodeGraph->DrawGraph();
    }

    void FMaterialEditorTool::DrawMaterialProperties(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialEditorTool::DrawMaterialPreview(const FUpdateContext& UpdateContext)
    {
    }

    void FMaterialEditorTool::DrawCompilationLog(const FUpdateContext& UpdateContext)
    {
        if (!CompilationResult.CompilationLog.empty())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            ImGui::BeginChild("CompilationLog", ImVec2(0, 0), true);

            ImVec4 Color = CompilationResult.bIsError ? ImVec4(1.0f, 0.2f, 0.2f, 1.0f)
                                                      : ImVec4(0.8f, 0.8f, 0.6f, 1.0f);

            ImGui::TextColored(Color, "%s", CompilationResult.CompilationLog.c_str());

            ImGui::EndChild();

            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
        }
    }
    
    void FMaterialEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        // Split horizontally: Left (Material Graph) and Right (Material Preview)
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        // Create a full bottom dock by splitting the main dockspace (InDockspaceID) only once
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        // Dock the windows into their respective locations
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialGraphName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialPreviewName).c_str(), rightDockID);

        // Dock only the MaterialCompileLogName window to the full bottom dock
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialCompileLogName).c_str(), bottomDockID);
    }


}
