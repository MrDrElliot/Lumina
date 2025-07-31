#include "MaterialEditorTool.h"

#include "LuminaEditor.h"
#include "imnodes/imnodes.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Renderer/RHIIncl.h"
#include "Core/Object/Class.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Renderer/MaterialTypes.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/ShaderCompiler.h"
#include "Scene/SceneManager.h"
#include "scene/entity/components/lightcomponent.h"
#include "scene/entity/components/staticmeshcomponent.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Thumbnails/ThumbnailManager.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "Tools/UI/ImGui/ImGuiColorTextEdit/TextEditor.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"
#include "UI/Tools/NodeGraph/Material/MaterialNodeGraph.h"

namespace Lumina
{
    static const char* MaterialGraphName           = "Material Graph";
    static const char* MaterialPropertiesName      = "Material Properties";
    static const char* GLSLPreviewName             = "GLSL Preview";

    FMaterialEditorTool::FMaterialEditorTool(IEditorToolContext* Context, CObject* InAsset)
        : FAssetEditorTool(Context, InAsset->GetName().c_str(), InAsset)
        , CompilationResult()
        , NodeGraph(nullptr)
    {
        FSceneManager* SceneManager = GEngine->GetEngineSubsystem<FSceneManager>();
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->RegisterSystem(NewObject<CDebugCameraEntitySystem>());

        Entity DirectionalLightEntity = NewScene->CreateEntity(FTransform(), "Directional Light");
        DirectionalLightEntity.AddComponent<SDirectionalLightComponent>();
        
        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
        MeshEntity.AddComponent<SStaticMeshComponent>();
        MeshEntity.GetComponent<SStaticMeshComponent>().StaticMesh = CThumbnailManager::Get().CubeMesh;
        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides.resize(CThumbnailManager::Get().CubeMesh->Materials.size());
        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides[0] = Cast<CMaterialInterface>(InAsset);
        
        Scene = NewScene;
    }


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

        CreateToolWindow(GLSLPreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawGLSLPreview(Cxt);
        });

        FString GraphName = Asset->GetName().ToString() + "MaterialGraph";
        NodeGraph = LoadObject<CMaterialNodeGraph>(Asset->GetPackage()->GetName(), FName(GraphName));
        if (NodeGraph == nullptr)
        {
            NodeGraph = NewObject<CMaterialNodeGraph>(Asset->GetPackage(), FName(GraphName));
        }
        
        NodeGraph->SetMaterial(Cast<CMaterial>(Asset.Get()));
        NodeGraph->Initialize();
        NodeGraph->SetNodeSelectedCallback( [this] (CEdGraphNode* Node)
        {
            if (Node != SelectedNode)
            {
                SelectedNode = Node;

                if (SelectedNode == nullptr)
                {
                    GetPropertyTable()->SetObject(Asset, CMaterial::StaticClass());
                }
                else
                {
                    GetPropertyTable()->SetObject(Node, Node->GetClass());
                }
            }
        });
    }
    
    void FMaterialEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        if (NodeGraph)
        {
            NodeGraph->Shutdown();
            NodeGraph = nullptr;
        }
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
        PropertyTable.DrawTree();
    }

    void FMaterialEditorTool::DrawMaterialPreview(const FUpdateContext& UpdateContext)
    {
        if (Asset == nullptr)
        {
            return;
        }
        
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
            ShaderCompiler->CompilerShaderRaw(Tree, {}, [this](const ShaderBinaries& Binaries)
            {
                IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
                FRHIPixelShaderRef PixelShader = RenderContext->CreatePixelShader(Binaries);

                FName Key = eastl::to_string(Hash::GetHash64(Binaries.data(), Binaries.size()));
                PixelShader->SetKey(Key);
                    
                FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader("Material.vert").As<FRHIVertexShader>();
                CMaterial* Material = Cast<CMaterial>(Asset.Get());
                Material->VertexShader = VertexShader;
                Material->PixelShader = PixelShader;
                RenderContext->OnShaderCompiled(PixelShader);
            });
            
            IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();

            CMaterial* Material = Cast<CMaterial>(Asset.Get());
            Compiler.GetBoundTextures(Material->Textures);

            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = sizeof(FMaterialUniforms);
            BufferDesc.DebugName = "Material Uniforms";
            BufferDesc.InitialState = EResourceStates::ConstantBuffer;
            BufferDesc.bKeepInitialState = true;
            BufferDesc.Usage.SetFlag(BUF_UniformBuffer);
            Material->UniformBuffer = RenderContext->CreateBuffer(BufferDesc);
            RenderContext->SetObjectName(Material->UniformBuffer, Material->GetName().c_str(), EAPIResourceType::Buffer);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::BufferCBV(0, Material->UniformBuffer));
            
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);

            FBindingLayoutItem BufferItem;
            BufferItem.Slot = 0;
            BufferItem.Type = ERHIBindingResourceType::Buffer_Uniform;
            LayoutDesc.AddItem(BufferItem);
            
            for (SIZE_T i = 0; i < Material->Textures.size(); ++i)
            {
                FRHIImageRef Image = Material->Textures[i]->RHIImage;
                
                FBindingLayoutItem Item;
                Item.Slot = i + 1; // Add 1 because uniform buffer is at 0.
                Item.Type = ERHIBindingResourceType::Texture_SRV;
                
                LayoutDesc.AddItem(Item);
                
                SetDesc.AddItem(FBindingSetItem::TextureSRV((uint32)i, Image));
            }
            

            Memory::Memzero(&Material->MaterialUniforms, sizeof(FMaterialUniforms));
            Material->Parameters.clear();
            
            for (const auto& [Name, Value] : Compiler.GetScalarParameters())
            {
                FMaterialParameter NewParam;
                NewParam.ParameterName = Name;
                NewParam.Type = EMaterialParameterType::Scalar;
                NewParam.Index = (uint16)Value.Index;
                Material->Parameters.push_back(NewParam);
                Material->SetScalarValue(Name, Value.Value);
            }

            for (const auto& [Name, Value] : Compiler.GetVectorParameters())
            {
                FMaterialParameter NewParam;
                NewParam.ParameterName = Name;
                NewParam.Type = EMaterialParameterType::Vector;
                NewParam.Index = (uint16)Value.Index;
                Material->Parameters.push_back(NewParam);
                Material->SetVectorValue(Name, Value.Value);
            }
            
            RenderContext->GetCommandList(ECommandQueue::Graphics)->WriteBuffer(Material->UniformBuffer, &Material->MaterialUniforms, 0, sizeof(FMaterialUniforms));
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
        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), rightDockID);

        // Dock only the MaterialCompileLogName window to the full bottom dock
        ImGui::DockBuilderDockWindow(GetToolWindowName(GLSLPreviewName).c_str(), bottomDockID);
    }


}
