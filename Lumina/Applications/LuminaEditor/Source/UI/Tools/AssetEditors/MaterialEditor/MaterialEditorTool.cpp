#include "MaterialEditorTool.h"
#include "imnodes/imnodes.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Renderer/RHIIncl.h"
#include "Core/Object/Class.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/ShaderCompiler.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "Tools/UI/ImGui/ImGuiColorTextEdit/TextEditor.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"
#include "UI/Tools/NodeGraph/Material/MaterialNodeGraph.h"

namespace Lumina
{
    const char* MaterialGraphName           = "Material Graph";
    const char* MaterialPropertiesName      = "Material Properties";
    const char* MaterialPreviewName         = "Material Preview";
    const char* GLSLPreviewName             = "GLSL Preview";

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

        CreateToolWindow(GLSLPreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawGLSLPreview(Cxt);
        });

        NodeGraph = NewObject<CMaterialNodeGraph>();
        NodeGraph->SetMaterial(static_cast<CMaterial*>(Asset));
        NodeGraph->Initialize();
    }


    void FMaterialEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        NodeGraph->Shutdown();
        NodeGraph->MarkGarbage();
        NodeGraph = nullptr;
    }

    void FMaterialEditorTool::OnAssetLoadFinished()
    {
    }
    
    void FMaterialEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::MenuItem(LE_ICON_RECEIPT_TEXT" Compile"))
        {
            Compile();
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
        if (Asset == nullptr)
        {
            return;
        }
        
        ImGuiX::DrawObjectProperties(Asset);

    }

    void FMaterialEditorTool::DrawGLSLPreview(const FUpdateContext& UpdateContext)
    {
        if (CompilationResult.CompilationLog.empty())
        {
            return;
        }

        TextEditor& Editor = IImGuiRenderer::GetTextEditor();

        if (bGLSLPreviewDirty)
        {
            Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
            Editor.SetText(CompilationResult.CompilationLog.c_str());
            Editor.SetReadOnly(true);
            Editor.SetShowWhitespaces(false);
            bGLSLPreviewDirty = false;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

        ImGui::BeginChild("CompilationLogEditor", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        Editor.Render("GLSL Compilation Log");
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }

    void FMaterialEditorTool::Compile()
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
            CompilationResult.CompilationLog = "Generated GLSL: \n \n \n" + Tree;
            CompilationResult.bIsError = false;

            bGLSLPreviewDirty = true;
            IShaderCompiler* ShaderCompiler = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext()->GetShaderCompiler();
            ShaderCompiler->CompilerShaderRaw(Tree, {}, [this](const TVector<uint32>& Binaries)
            {
                IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
                FRHIPixelShaderRef PixelShader = RenderContext->CreatePixelShader(Binaries);

                FName Key = eastl::to_string(Hash::GetHash64(Binaries.data(), Binaries.size())).c_str();
                PixelShader->SetKey(Key);
                    
                FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader("Material.vert").As<FRHIVertexShader>();
                CMaterial* Material = Cast<CMaterial>(Asset);
                Material->VertexShader = VertexShader;
                Material->PixelShader = PixelShader;
                RenderContext->OnShaderCompiled(PixelShader);
            });

            IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();

            CMaterial* Material = Cast<CMaterial>(Asset);
            Compiler.GetBoundImages(Material->Images);
            
            FBindingSetDesc SetDesc;
            
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
            
            for (int i = 0; i < Material->Images.size(); ++i)
            {
                FRHIImageRef Image = Material->Images[i];
                
                FBindingLayoutItem Item;
                Item.Slot = i;
                Item.Type = ERHIBindingResourceType::Texture_SRV;
                
                LayoutDesc.AddItem(Item);
                SetDesc.AddItem(FBindingSetItem::TextureSRV(i, Image));
            }

            Material->BindingLayout = RenderContext->CreateBindingLayout(LayoutDesc);
            RenderContext->SetObjectName(Material->BindingLayout, Material->GetName().c_str(), EAPIResourceType::DescriptorSetLayout);
            
            Material->BindingSet = RenderContext->CreateBindingSet(SetDesc, Material->BindingLayout);
            RenderContext->SetObjectName(Material->BindingSet, Material->GetName().c_str(), EAPIResourceType::DescriptorSet);

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
        ImGui::DockBuilderDockWindow(GetToolWindowName(GLSLPreviewName).c_str(), bottomDockID);
    }


}
