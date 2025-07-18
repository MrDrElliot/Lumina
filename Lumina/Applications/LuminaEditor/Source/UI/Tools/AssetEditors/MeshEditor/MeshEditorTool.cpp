#include "MeshEditorTool.h"

#include "ImGuiDrawUtils.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "glm/gtc/type_ptr.inl"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderTypes.h"
#include "Scene/Entity/Components/LightComponent.h"
#include "Scene/Entity/Components/StaicMeshComponent.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Tools/UI/ImGui/ImGuiX.h"


namespace Lumina
{
    const char* MeshPropertiesName        = "MeshProperties";

    FMeshEditorTool::FMeshEditorTool(IEditorToolContext* Context, CObject* InAsset)
    : FAssetEditorTool(Context, InAsset->GetName().c_str(), InAsset)
    {
        FSceneManager* SceneManager = GEngine->GetEngineSubsystem<FSceneManager>();
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->RegisterSystem(Memory::New<FDebugCameraEntitySystem>());

        Entity DirectionalLightEntity = NewScene->CreateEntity(FTransform(), "Directional Light");
        DirectionalLightEntity.AddComponent<FDirectionalLightComponent>();

        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
        FStaticMeshComponent& NewComponent = MeshEntity.AddComponent<FStaticMeshComponent>();
        NewComponent.StaticMesh = Cast<CStaticMesh>(InAsset);
        
        Scene = NewScene;
    }

    void FMeshEditorTool::OnInitialize()
    {
        CreateToolWindow(MeshPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CStaticMesh* StaticMesh = Cast<CStaticMesh>(Asset);

            ImGui::Text("Vertices: %u", StaticMesh->GetNumVertices());
            ImGui::Text("Indices: %u", StaticMesh->GetNumIndices());

            ImGui::Separator();

            
            ImGui::Separator();

            auto& Registry = Scene->GetMutableEntityRegistry();
            auto View = Registry.view<FDirectionalLightComponent, FTransformComponent>();

            for (auto Entity : View)
            {
                auto& DLC = Registry.get<FDirectionalLightComponent>(Entity);
                auto& TC  = Registry.get<FTransformComponent>(Entity);


                if (ImGui::TreeNode("Directional Light"))
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::ColorPicker3("Color", glm::value_ptr(DLC.Color),
                                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);

                    ImGui::SliderFloat("Intensity", &DLC.Color.a, 0.1f, 200.0f);

                    if (ImGui::DragFloat3("Direction", glm::value_ptr(DLC.Direction)))
                    {
                        
                    }

                    ImGui::TreePop();
                }
            }

            ImGuiX::DrawObjectProperties(StaticMesh);
        });
    }

    void FMeshEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FMeshEditorTool::OnAssetLoadFinished()
    {
    }

    void FMeshEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        FAssetEditorTool::DrawToolMenu(UpdateContext);
    }

    void FMeshEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MeshPropertiesName).c_str(), rightDockID);
    }
}
