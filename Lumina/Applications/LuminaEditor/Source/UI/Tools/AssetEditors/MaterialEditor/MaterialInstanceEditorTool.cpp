#include "MaterialInstanceEditorTool.h"

#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Material/MaterialInstance.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "glm/gtc/type_ptr.inl"
#include "Renderer/RenderContext.h"
#include "Renderer/RHIGlobals.h"
#include "World/entity/components/lightcomponent.h"
#include "World/entity/components/staticmeshcomponent.h"
#include "World/Entity/Systems/EditorEntityMovementSystem.h"
#include "thumbnails/thumbnailmanager.h"

namespace Lumina
{
    static const char* MaterialEditorName          = "Material Editor";

    FMaterialInstanceEditorTool::FMaterialInstanceEditorTool(IEditorToolContext* Context, CObject* InAsset)
        : FAssetEditorTool(Context, InAsset->GetName().c_str(), InAsset)
    {
//        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
//        NewScene->RegisterSystem(NewObject<CDebugCameraEntitySystem>());
//
//        Entity DirectionalLightEntity = NewScene->CreateEntity(FTransform(), "Directional Light");
//        DirectionalLightEntity.AddComponent<SDirectionalLightComponent>();
//        
//        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
//        MeshEntity.AddComponent<SStaticMeshComponent>();
//        
//        MeshEntity.GetComponent<SStaticMeshComponent>().StaticMesh = CThumbnailManager::Get().CubeMesh;
//        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides.resize(CThumbnailManager::Get().CubeMesh->Materials.size());
//        MeshEntity.GetComponent<SStaticMeshComponent>().MaterialOverrides[0] = Cast<CMaterialInterface>(InAsset);
//
        //Scene = NewScene;
    }
    void FMaterialInstanceEditorTool::OnInitialize()
    {
        CreateToolWindow(MaterialEditorName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            PropertyTable.DrawTree();
        
            ICommandList* CommandList = GRenderContext->GetCommandList(ECommandQueue::Graphics);
        
            CMaterialInstance* Instance = Cast<CMaterialInstance>(Asset.Get());
            if (!Instance || !Instance->Material.IsValid())
                return;
        
            ImGui::PushStyleColor(ImGuiCol_Header, 0);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, 0);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
            if (ImGui::CollapsingHeader("Material Parameters"))
            {
                constexpr ImGuiTableFlags Flags = 
                    ImGuiTableFlags_BordersOuter | 
                    ImGuiTableFlags_BordersInnerH | 
                    ImGuiTableFlags_NoBordersInBodyUntilResize | 
                    ImGuiTableFlags_SizingFixedFit;
        
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 8));
                if (ImGui::BeginTable("MaterialParamsTable", 2, Flags))
                {
                    ImGui::TableSetupColumn("##Header", ImGuiTableColumnFlags_WidthFixed, 175);
                    ImGui::TableSetupColumn("##Editor", ImGuiTableColumnFlags_WidthStretch);
        
                    
                    for (FMaterialParameter& Param : Instance->Parameters)
                    {
                        ImGui::PushID(&Param);
                        ImGui::TableNextRow();
                        
                        ImGui::TableNextColumn();
                        
                        ImGui::TextUnformatted(Param.ParameterName.c_str());
        
                        ImGui::TableNextColumn();
                        
                        ImGui::AlignTextToFramePadding();
                        
                        switch (Param.Type)
                        {
                            case EMaterialParameterType::Scalar:
                                {
                                    int VecIndex = Param.Index / 4;
                                    int ComponentIndex = Param.Index % 4;
                                    float* ValuePtr = &Instance->MaterialUniforms.Scalars[VecIndex][ComponentIndex];
        
                                    ImGui::PushID(&Param);
                                    if (ImGui::DragFloat("##Scalar", ValuePtr, 0.1f))
                                    {
                                        CommandList->WriteBuffer(Instance->UniformBuffer, &Instance->MaterialUniforms, 0, sizeof(FMaterialUniforms));
                                    }
                                    ImGui::PopID();
                                    break;
                                }
        
                                case EMaterialParameterType::Vector:
                                {
                                    glm::vec4& Vec = Instance->MaterialUniforms.Vectors[Param.Index];
                                    ImGui::PushID(&Param);
                                    if (ImGui::ColorEdit4("##Vector", glm::value_ptr(Vec), ImGuiColorEditFlags_Float))
                                    {
                                        CommandList->WriteBuffer(Instance->UniformBuffer, &Instance->MaterialUniforms, 0, sizeof(FMaterialUniforms));
                                    }
                                    ImGui::PopID();
                                    break;
                                }
        
                                case EMaterialParameterType::Texture:
                            ImGui::TextUnformatted("Texture editing not implemented");
                            break;
                        }
        
                        ImGui::PopID();
                    }
        
                    
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();
            }
            
            ImGui::PopStyleColor(3);
        
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
        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MaterialEditorName).c_str(), rightDockID);
    }
    
}
