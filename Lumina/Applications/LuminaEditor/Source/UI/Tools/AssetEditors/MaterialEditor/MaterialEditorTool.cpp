#include "MaterialEditorTool.h"
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
#include "Renderer/RHIGlobals.h"
#include "Renderer/ShaderCompiler.h"
#include "World/entity/components/lightcomponent.h"
#include "World/entity/components/staticmeshcomponent.h"
#include "World/Entity/Systems/EditorEntityMovementSystem.h"
#include "Thumbnails/ThumbnailManager.h"
#include "Tools/UI/ImGui/ImGuiX.h"
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
        World = NewObject<CWorld>();

        Entity DirectionalLightEntity = World->ConstructEntity("Directional Light");
        DirectionalLightEntity.Emplace<SDirectionalLightComponent>();
        
        MeshEntity = World->ConstructEntity("MeshEntity");
        
        MeshEntity.Emplace<SStaticMeshComponent>().StaticMesh = Cast<CStaticMesh>(InAsset);
        MeshEntity.GetComponent<STransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, -2.5f));
        
        MeshEntity.GetComponent<SStaticMeshComponent>().StaticMesh = CThumbnailManager::Get().CubeMesh;
        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides.resize(CThumbnailManager::Get().CubeMesh->Materials.size());
        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides[0] = Cast<CMaterialInterface>(InAsset);
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

        FString GraphName = Asset->GetName().ToString() + "_MaterialGraph";
        NodeGraph = LoadObject<CMaterialNodeGraph>(Asset->GetPackage(), GraphName);
        if (NodeGraph == nullptr)
        {
            NodeGraph = NewObject<CMaterialNodeGraph>(Asset->GetPackage(), GraphName);
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
            
            IShaderCompiler* ShaderCompiler = GRenderContext->GetShaderCompiler();
            FShaderHeader CompiledHeader;
            ShaderCompiler->CompilerShaderRaw(Tree, {}, [this, &CompiledHeader](const FShaderHeader& Header) mutable 
            {
                CompiledHeader = Header;
                
                FRHIPixelShaderRef PixelShader = GRenderContext->CreatePixelShader(Header);
                
                FName Key = eastl::to_string(Hash::GetHash64(Header.Binaries.data(), Header.Binaries.size()));
                PixelShader->SetKey(Key);
                    
                FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader("Material.vert").As<FRHIVertexShader>();
                CMaterial* Material = Cast<CMaterial>(Asset.Get());
                Material->VertexShader = VertexShader;
                Material->PixelShader = PixelShader;
                
                GRenderContext->OnShaderCompiled(PixelShader);
            });
            
            ShaderCompiler->Flush();
            
            CMaterial* Material = Cast<CMaterial>(Asset.Get());
            Compiler.GetBoundTextures(Material->Textures);
            
            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = sizeof(FMaterialUniforms);
            BufferDesc.DebugName = "Material Uniforms";
            BufferDesc.InitialState = EResourceStates::ConstantBuffer;
            BufferDesc.bKeepInitialState = true;
            BufferDesc.Usage.SetFlag(BUF_UniformBuffer);
            Material->UniformBuffer = GRenderContext->CreateBuffer(BufferDesc);
            GRenderContext->SetObjectName(Material->UniformBuffer, Material->GetName().c_str(), EAPIResourceType::Buffer);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::BufferCBV(0, Material->UniformBuffer));
            
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);

            FBindingLayoutItem BufferItem;
            BufferItem.Slot = 0;
            BufferItem.Type = ERHIBindingResourceType::Buffer_Uniform;
            LayoutDesc.AddItem(BufferItem);

            for (const FShaderBinding& Binding : CompiledHeader.Reflection.Bindings)
            {
                if (Binding.Type == ERHIBindingResourceType::Texture_SRV)
                {
                    FRHIImageRef Image = Material->Textures[Binding.Set - 1]->RHIImage;
                
                    FBindingLayoutItem Item;
                    Item.Slot = Binding.Binding;
                    Item.Type = ERHIBindingResourceType::Texture_SRV;
                
                    LayoutDesc.AddItem(Item);
                
                    SetDesc.AddItem(FBindingSetItem::TextureSRV(Item.Slot, Image));
                }
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
            
            GRenderContext->GetCommandList(ECommandQueue::Graphics)->WriteBuffer(Material->UniformBuffer, &Material->MaterialUniforms, 0, sizeof(FMaterialUniforms));
            Material->BindingLayout = GRenderContext->CreateBindingLayout(LayoutDesc);
            
            GRenderContext->SetObjectName(Material->BindingLayout, Material->GetName().c_str(), EAPIResourceType::DescriptorSetLayout);
            
            Material->BindingSet = GRenderContext->CreateBindingSet(SetDesc, Material->BindingLayout);
            GRenderContext->SetObjectName(Material->BindingSet, Material->GetName().c_str(), EAPIResourceType::DescriptorSet);

            OnSave();
        }
    }


    void FMaterialEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0;
        ImGuiID rightBottomDockID = 0;

        // 1. Split horizontally: Left (Material Graph) and Right (Material Preview + bottom)
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        // 2. Split right dock vertically: Top (Material Preview), Bottom (GLSL Preview)
        ImGui::DockBuilderSplitNode(rightDockID, ImGuiDir_Down, 0.3f, &rightBottomDockID, &rightDockID);

        // Dock windows
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialGraphName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), rightDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(GLSLPreviewName).c_str(), rightBottomDockID);
    }
}
