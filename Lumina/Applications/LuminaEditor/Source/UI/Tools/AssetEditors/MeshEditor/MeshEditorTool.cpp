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
#include "Tools/UI/ImGui/ImGuiFonts.h"
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
        DirectionalLightEntity.AddComponent<FNeedsRenderProxyUpdate>();
        
        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
        
        MeshEntity.AddComponent<FStaticMeshComponent>().StaticMesh = Cast<CStaticMesh>(InAsset);
        MeshEntity.GetComponent<FTransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, -2.5f));
        MeshEntity.AddComponent<FNeedsRenderProxyUpdate>();
        
        Scene = NewScene;
    }

    void FMeshEditorTool::OnInitialize()
    {
        CreateToolWindow(MeshPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CStaticMesh* StaticMesh = Cast<CStaticMesh>(Asset);

            const FMeshResource& Resource = StaticMesh->GetMeshResource();
            TVector<FGeometrySurface> Surfaces = Resource.GeometrySurfaces;
            
            if (ImGui::CollapsingHeader("Mesh Resources"))
            {
                ImGui::Indent();

                if (ImGui::BeginTable("MeshResourceTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    auto Row = [](const char* label, const FString& value)
                    {
                           ImGui::TableNextRow();
                           ImGui::TableSetColumnIndex(0);
                           ImGui::TextUnformatted(label);
                           ImGui::TableSetColumnIndex(1);
                           ImGui::TextUnformatted(value.c_str());
                    };

                    Row("Vertices", eastl::to_string(Resource.Vertices.size()));
                    Row("Indices", eastl::to_string(Resource.Indices.size()));
                    Row("Surfaces", eastl::to_string(Resource.GetNumSurfaces()));

                    ImGui::EndTable();
                }

                ImGui::Unindent();
            }

            ImGuiX::Font::PushFont(ImGuiX::Font::EFont::Large);
            ImGui::SeparatorText("Geometry Surfaces");
            ImGuiX::Font::PopFont();
            
            for (size_t i = 0; i < Resource.GeometrySurfaces.size(); ++i)
            {
                const FGeometrySurface& Surface = Resource.GeometrySurfaces[i];
                ImGui::PushID(i);
                ImGui::Text("Name: %s", Surface.ID.c_str());
                ImGui::Separator();
                ImGui::PopID();
            }

            ImGui::SeparatorText("Asset Details");
            
            ImGuiX::DrawObjectProperties(StaticMesh);
            
            ImGui::SeparatorText("Directional Light Details");

            
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
