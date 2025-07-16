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

        int gridSize = sqrt(2000); // ≈ sqrt(1000)
        float spacing = 2.0f;

        for (int i = 0; i < 2000; ++i)
        {
            int x = i % gridSize;
            int y = i / gridSize;

            glm::vec3 position = glm::vec3(x * spacing, y * spacing, 0.0f);

            MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
            FStaticMeshComponent& NewComponent = MeshEntity.AddComponent<FStaticMeshComponent>();

            MeshEntity.GetComponent<FTransformComponent>().SetLocation(position);
            NewComponent.StaticMesh = Cast<CStaticMesh>(InAsset);
        }
        
        Scene = NewScene;
    }

    void FMeshEditorTool::OnInitialize()
    {
        CreateToolWindow(MeshPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CStaticMesh* StaticMesh = Cast<CStaticMesh>(Asset);

            ImGui::Text("Vertices: %u", StaticMesh->GetNumVertices());
            ImGui::Text("Indices: %u", StaticMesh->GetNumIndicies());

            ImGui::Separator();

            auto& Registry = Scene->GetMutableEntityRegistry();

            // Add Light Button
            if (ImGui::Button("Add Point Light"))
            {
                Entity New = Scene->CreateEntity(FTransform(), "New Light");
                New.AddComponent<FPointLightComponent>();
            }

            ImGui::Separator();

            auto View = Registry.view<FPointLightComponent, FTransformComponent>();

            int lightIndex = 0;
            for (auto Entity : View)
            {
                auto& PLC = Registry.get<FPointLightComponent>(Entity);
                auto& TC  = Registry.get<FTransformComponent>(Entity);

                ImGui::PushID(lightIndex++);

                if (ImGui::TreeNode("Point Light"))
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::ColorPicker3("Color", glm::value_ptr(PLC.LightColor),
                                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);

                    ImGui::SliderFloat("Intensity", &PLC.LightColor.a, 0.1, 10000.0f);

                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGuiUtils::DrawVec3Control("Position", TC.Transform.Location);
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
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
